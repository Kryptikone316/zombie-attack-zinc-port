#define _GNU_SOURCE

#include "hwdisp.h"
#include "hwaudio.h"
#include "joykey.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GAME_W 640
#define GAME_H 480
#define ASSET_COUNT 31
#define ASSET_FARM 0
#define ASSET_COMMANDO 1
#define ASSET_ZOMBIE 2
#define ASSET_BULLET 3
#define ASSET_PICKUP_HEALTH 4
#define ASSET_PICKUP_DAMAGE 5
#define ASSET_STATUS_DAMAGE 6
#define ASSET_STATUS_POISON 7
#define ASSET_STATUS_CRIPPLE 8
#define ASSET_ANIMAL_CHICKEN 9
#define ASSET_ANIMAL_PIG 10
#define ASSET_ANIMAL_SHEEP 11
#define ASSET_ANIMAL_GOAT 12
#define ASSET_ANIMAL_COW 13
#define ASSET_ANIMAL_LLAMA 14
#define ASSET_ANIMAL_UNICORN 15
#define ASSET_PICKUP_SPEED 16
#define ASSET_PICKUP_RAPID 17
#define ASSET_STATUS_SPEED 18
#define ASSET_STATUS_RAPID 19
#define ASSET_COW_PIE 20
#define ASSET_ANIMAL_CROW 21
#define ASSET_RESCUE_PIG 22
#define ASSET_WOLF 23
#define ASSET_DOG 24
#define ASSET_PICKUP_AMMO 25
#define ASSET_MOWER 26
#define ASSET_CHIPPER 27
#define ASSET_SQUIRREL 28
#define ASSET_BAD_NUT 29
#define ASSET_ICON 30

typedef struct {
    int w;
    int h;
    unsigned short *pixels;
} image_t;

static unsigned short framebuffer[GAME_W * GAME_H];
static image_t assets[ASSET_COUNT];
static int display_active;
static int audio_active;
static char data_root[512];
static int sound_volume = 5;
static int music_volume = 5;
static int global_volume = 5;
static const unsigned short transparent = 0xf81f;

static unsigned short rd16(const unsigned char *p)
{
    return (unsigned short)(p[0] | ((unsigned short)p[1] << 8));
}

static unsigned int rd32(const unsigned char *p)
{
    return (unsigned int)p[0] |
           ((unsigned int)p[1] << 8) |
           ((unsigned int)p[2] << 16) |
           ((unsigned int)p[3] << 24);
}

static unsigned short rgb565(unsigned char r, unsigned char g, unsigned char b)
{
    return (unsigned short)(((unsigned short)(r >> 3) << 11) |
                            ((unsigned short)(g >> 2) << 5) |
                            (unsigned short)(b >> 3));
}

static int load_bmp(const char *path, image_t *image)
{
    FILE *file;
    unsigned char *data;
    long length;
    unsigned int pixels_offset, dib_size, colors_used;
    int width, height, top_down, bpp, x, y;
    unsigned int row_bytes;

    memset(image, 0, sizeof(*image));
    file = fopen(path, "rb");
    if (!file) return -1;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return -1; }
    length = ftell(file);
    if (length < 64 || fseek(file, 0, SEEK_SET) != 0) { fclose(file); return -1; }
    data = (unsigned char *)malloc((size_t)length);
    if (!data) { fclose(file); return -1; }
    if (fread(data, 1, (size_t)length, file) != (size_t)length) {
        free(data); fclose(file); return -1;
    }
    fclose(file);

    if (data[0] != 'B' || data[1] != 'M') { free(data); return -1; }
    pixels_offset = rd32(data + 10);
    dib_size = rd32(data + 14);
    if (dib_size < 40 || 14 + dib_size > (unsigned int)length) { free(data); return -1; }
    width = (int)rd32(data + 18);
    height = (int)rd32(data + 22);
    bpp = (int)rd16(data + 28);
    colors_used = rd32(data + 46);
    top_down = height < 0;
    if (height < 0) height = -height;
    if (width <= 0 || height <= 0 || (bpp != 1 && bpp != 4 && bpp != 8) ||
        rd32(data + 30) != 0 || pixels_offset >= (unsigned int)length) {
        free(data); return -1;
    }

    image->pixels = (unsigned short *)malloc((size_t)width * (size_t)height * sizeof(unsigned short));
    if (!image->pixels) { free(data); return -1; }
    image->w = width;
    image->h = height;
    if (!colors_used) colors_used = 1u << bpp;
    row_bytes = ((unsigned int)width * (unsigned int)bpp + 31u) / 32u * 4u;

    for (y = 0; y < height; y++) {
        int source_y = top_down ? y : height - 1 - y;
        const unsigned char *row = data + pixels_offset + (size_t)source_y * row_bytes;
        for (x = 0; x < width; x++) {
            unsigned int index;
            unsigned int palette_offset;
            unsigned char b, g, r;
            if (bpp == 8) index = row[x];
            else if (bpp == 4) index = (row[x / 2] >> ((x & 1) ? 0 : 4)) & 15u;
            else index = (row[x / 8] >> (7 - (x & 7))) & 1u;
            if (index >= colors_used) index = 0;
            palette_offset = 14u + dib_size + index * 4u;
            if (palette_offset + 2u >= (unsigned int)length) {
                b = g = r = 0;
            } else {
                b = data[palette_offset];
                g = data[palette_offset + 1];
                r = data[palette_offset + 2];
            }
            image->pixels[y * width + x] = rgb565(r, g, b);
        }
    }
    free(data);
    return 0;
}

static void release_assets(void)
{
    int i;
    for (i = 0; i < ASSET_COUNT; i++) {
        free(assets[i].pixels);
        assets[i].pixels = NULL;
    }
}

static unsigned short tint_pixel(unsigned short color, int tint)
{
    unsigned int r = (color >> 11) & 31u;
    unsigned int g = (color >> 5) & 63u;
    unsigned int b = color & 31u;
    if (tint == 1) {
        r = r + 8u > 31u ? 31u : r + 8u;
        g = g > 14u ? g - 14u : 0u;
        b = b > 8u ? b - 8u : 0u;
    } else if (tint == 2) {
        b = b + 10u > 31u ? 31u : b + 10u;
        r = r > 6u ? r - 6u : 0u;
        g = g > 8u ? g - 8u : 0u;
    } else if (tint == 3) {
        g = g + 12u > 63u ? 63u : g + 12u;
        r = r > 5u ? r - 5u : 0u;
        b = b > 5u ? b - 5u : 0u;
    }
    return (unsigned short)((r << 11) | (g << 5) | b);
}

static void draw_image_tinted(int asset, int sx, int sy, int sw, int sh,
                              int dx, int dy, int flip, int tint)
{
    image_t *image = &assets[asset];
    int x, y;
    if (!image->pixels) return;
    for (y = 0; y < sh; y++) {
        int py = dy + y;
        int source_y = sy + y;
        if (py < 0 || py >= GAME_H || source_y < 0 || source_y >= image->h) continue;
        for (x = 0; x < sw; x++) {
            int px = dx + x;
            int source_x = flip ? sx + sw - 1 - x : sx + x;
            unsigned short color;
            if (px < 0 || px >= GAME_W || source_x < 0 || source_x >= image->w) continue;
            color = image->pixels[source_y * image->w + source_x];
            if (color != transparent)
                framebuffer[py * GAME_W + px] = tint_pixel(color, tint);
        }
    }
}

static void draw_image(int asset, int sx, int sy, int sw, int sh,
                       int dx, int dy, int flip)
{
    draw_image_tinted(asset, sx, sy, sw, sh, dx, dy, flip, 0);
}

static void fill_rect(int x, int y, int w, int h, unsigned short color)
{
    int px, py;
    for (py = y; py < y + h; py++) {
        if (py < 0 || py >= GAME_H) continue;
        for (px = x; px < x + w; px++) {
            if (px >= 0 && px < GAME_W) framebuffer[py * GAME_W + px] = color;
        }
    }
}

static void darken(void)
{
    int i;
    for (i = 0; i < GAME_W * GAME_H; i++) {
        unsigned short p = framebuffer[i];
        unsigned int r = (p >> 11) & 31u;
        unsigned int g = (p >> 5) & 63u;
        unsigned int b = p & 31u;
        framebuffer[i] = (unsigned short)(((r / 2u) << 11) | ((g / 2u) << 5) | (b / 2u));
    }
}

static const char *const font_data[41] = {
    "01110100011000111111100011000110001", "11110100011000111110100011000111110",
    "01111100001000010000100001000001111", "11110100011000110001100011000111110",
    "11111100001000011110100001000011111", "11111100001000011110100001000010000",
    "01111100001000010111100011000101111", "10001100011000111111100011000110001",
    "11111001000010000100001000010011111", "00111000100001000010000100001001110",
    "10001100101010011000101001001010001", "10000100001000010000100001000011111",
    "10001110111010110101100011000110001", "10001110011010110011100011000110001",
    "01110100011000110001100011000101110", "11110100011000111110100001000010000",
    "01110100011000110001101011001001101", "11110100011000111110101001001010001",
    "01111100001000001110000011000111110", "11111001000010000100001000010000100",
    "10001100011000110001100011000101110", "10001100011000110001100011000101010",
    "10001100011000110101101011101110001", "10001100010101000100010101000110001",
    "10001100010101000100001000010000100", "11111000010001000100010001000011111",
    "01110100011000110001100011000101110", "00100011000010000100001000010001110",
    "01110100010000100010001000100011111", "11110000010000101110000010000111110",
    "00010001100101010010111110001000010", "11111100001000011110000010000111110",
    "01110100001000011110100011000101110", "11111000010001000100010000100001000",
    "01110100011000101110100011000101110", "01110100011000101111000010000101110",
    "00000001000010011111001000010000000", "00000000000000011111000000000000000",
    "00100001000010000100001000000000100",
    "00100001000000000000000000000000000", "00000000000000000000000000000000100"
};

static int font_index(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= '0' && c <= '9') return 26 + c - '0';
    if (c == '+') return 36;
    if (c == '-') return 37;
    if (c == '!') return 38;
    if (c == '\'') return 39;
    if (c == '.') return 40;
    return 35;
}

static void draw_text_color(int x, int y, const char *text, int scale,
                            unsigned short color)
{
    int i, row, col;
    for (i = 0; text[i]; i++) {
        int index = font_index(text[i]);
        if (text[i] == ' ') { x += 6 * scale; continue; }
        for (row = 0; row < 7; row++) {
            for (col = 0; col < 5; col++) {
                if (font_data[index][row * 5 + col] == '1')
                    fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
        x += 6 * scale;
    }
}

static void draw_text(int x, int y, const char *text, int scale)
{
    draw_text_color(x, y, text, scale, 0xffff);
}

static void draw_number(int x, int y, int value)
{
    char text[16];
    snprintf(text, sizeof(text), "%d", value);
    draw_text(x, y, text, 1);
}

static void draw_number_scaled(int x, int y, int value, int scale)
{
    char text[16];
    snprintf(text, sizeof(text), "%d", value);
    draw_text(x, y, text, scale);
}

static void draw_time_icon(int x, int y, int night)
{
    if (!night) {
        fill_rect(x + 5, y + 4, 8, 8, 0xffe0);
        fill_rect(x + 7, y, 4, 16, 0xffe0);
        fill_rect(x, y + 6, 18, 4, 0xffe0);
        fill_rect(x + 2, y + 2, 3, 3, 0xffe0);
        fill_rect(x + 13, y + 2, 3, 3, 0xffe0);
        fill_rect(x + 2, y + 11, 3, 3, 0xffe0);
        fill_rect(x + 13, y + 11, 3, 3, 0xffe0);
    } else {
        /* Cut a dark crescent from a pale moon using the known HUD color. */
        fill_rect(x + 4, y + 1, 11, 14, 0xffe0);
        fill_rect(x + 9, y, 9, 14, 0x2104);
        fill_rect(x + 3, y + 4, 4, 7, 0xffe0);
    }
}

static void audio_tone(int id);
void zombie_host_set_volumes(int sound, int music, int global);

int zombie_host_init(int unused)
{
    const char *data_dir = getenv("ZOMBIE_DATA");
    char path[512];
    const char *names[ASSET_COUNT] = {
        "farm.bmp", "commando.bmp", "zombie.bmp", "bullet.bmp",
        "pickup-health.bmp", "pickup-damage.bmp", "status-damage.bmp",
        "status-poison.bmp", "status-cripple.bmp", "animal-chicken.bmp",
        "animal-pig.bmp", "animal-sheep.bmp", "animal-goat.bmp",
        "animal-cow.bmp", "animal-llama.bmp", "animal-unicorn.bmp",
        "pickup-speed.bmp", "pickup-rapid.bmp", "status-speed.bmp",
        "status-rapid.bmp", "cow-pie.bmp", "animal-crow.bmp",
        "rescue-piglet.bmp", "big-bad-wolf.bmp", "beagle-buddy.bmp",
        "pickup-ammo.bmp", "lawn-mower.bmp", "wood-chipper.bmp",
        "squirrel.bmp", "bad-nut.bmp", "icon.bmp"
    };
    int i;
    (void)unused;

    if (!data_dir || !*data_dir) data_dir = "./data";
    snprintf(data_root, sizeof(data_root), "%s", data_dir);
    for (i = 0; i < ASSET_COUNT; i++) {
        snprintf(path, sizeof(path), "%s/%s", data_dir, names[i]);
        if (load_bmp(path, &assets[i]) != 0) {
            fprintf(stderr, "zombie: unable to load %s\n", path);
            release_assets();
            return -1;
        }
    }
    memset(framebuffer, 0, sizeof(framebuffer));
    joykey_init();
    display_active = hwdisp_init() == 0;
    if (display_active) {
        hwdisp_set_target_aspect(4, 3);
        /* The shared shim's nearest path is intentionally not used: its
         * staging buffer is reserved for a future scaler implementation. */
        hwdisp_set_filter(0);
    }
    audio_active = hwaudio_init() == 0;
    fprintf(stderr, "zombie: Zinc runtime display=%s audio=%s\n",
            display_active ? "active" : "unavailable",
            audio_active ? "active" : "unavailable");
    return 0;
}

void zombie_host_deinit(int unused)
{
    (void)unused;
    if (audio_active) hwaudio_deinit();
    if (display_active) hwdisp_deinit();
    release_assets();
}

unsigned int zombie_host_settings_load(int unused)
{
    char path[512];
    FILE *file;
    int sound = 5, music = 5, global = 5;
    (void)unused;
    snprintf(path, sizeof(path), "%s/settings.cfg", data_root[0] ? data_root : ".");
    file = fopen(path, "r");
    if (file) {
        if (fscanf(file, "%d %d %d", &sound, &music, &global) != 3) {
            sound = music = global = 5;
        }
        fclose(file);
    }
    if (sound < 0 || sound > 5) sound = 5;
    if (music < 0 || music > 5) music = 5;
    if (global < 0 || global > 5) global = 5;
    sound_volume = sound;
    music_volume = music;
    global_volume = global;
    return (unsigned int)sound | ((unsigned int)music << 8) |
           ((unsigned int)global << 16);
}

void zombie_host_settings_save(int sound, int music, int global)
{
    char path[512];
    FILE *file;
    zombie_host_set_volumes(sound, music, global);
    snprintf(path, sizeof(path), "%s/settings.cfg", data_root[0] ? data_root : ".");
    file = fopen(path, "w");
    if (!file) return;
    fprintf(file, "%d %d %d\n", sound_volume, music_volume, global_volume);
    fclose(file);
}

void zombie_host_set_volumes(int sound, int music, int global)
{
    if (sound < 0) sound = 0;
    if (sound > 5) sound = 5;
    if (music < 0) music = 0;
    if (music > 5) music = 5;
    if (global < 0) global = 0;
    if (global > 5) global = 5;
    sound_volume = sound;
    music_volume = music;
    global_volume = global;
}

unsigned int zombie_host_keys(int unused)
{
    (void)unused;
    return joykey_read();
}

unsigned int zombie_host_time_ms(int unused)
{
    struct timespec ts;
    (void)unused;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned int)(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
}

void zombie_host_sleep(int milliseconds)
{
    if (milliseconds > 0) usleep((useconds_t)milliseconds * 1000u);
}

void zombie_host_background(int night)
{
    if (assets[ASSET_FARM].pixels && assets[ASSET_FARM].w == GAME_W && assets[ASSET_FARM].h == GAME_H)
        memcpy(framebuffer, assets[ASSET_FARM].pixels, sizeof(framebuffer));
    else
        fill_rect(0, 0, GAME_W, GAME_H, 0x3186);
    if (night) darken();
}

void zombie_host_laser(int x, int y, int direction, int stop_x)
{
    int start;
    int width;
    int beam_y = y - 46;
    if (direction == 0) {
        start = stop_x;
        width = x - stop_x;
    } else {
        start = x;
        width = stop_x - x;
    }
    if (width <= 0) return;
    fill_rect(start, beam_y - 1, width, 1, 0xf800);
    fill_rect(start, beam_y, width, 1, 0xfbe0);
}

void zombie_host_player(int x, int y, int frame, int direction, int state)
{
    int sx, sy, sw;
    if (state == 2) { sx = (frame & 1) * 57; sy = 146; sw = 57; }
    else if (state == 1) { sx = (frame & 7) * 48; sy = 0; sw = 48; }
    else { sx = (frame % 3) * 48; sy = 73; sw = 48; }
    draw_image(ASSET_COMMANDO, sx, sy, sw, 73, x - sw / 2, y - 73, direction == 0);
}

void zombie_host_player_status(int x, int y, int damage, int poison, int cripple,
                               int speed, int rapid)
{
    int slot = 0;
    int count = damage + poison + cripple + speed + rapid;
    int start = x - count * 12;
    if (damage) {
        draw_image(ASSET_STATUS_DAMAGE, 0, 0, 24, 24,
                   start + slot * 24, y - 98, 0);
        ++slot;
    }
    if (poison) {
        draw_image(ASSET_STATUS_POISON, 0, 0, 24, 24,
                   start + slot * 24, y - 98, 0);
        ++slot;
    }
    if (cripple) {
        draw_image(ASSET_STATUS_CRIPPLE, 0, 0, 24, 24,
                   start + slot * 24, y - 98, 0);
        ++slot;
    }
    if (speed) {
        draw_image(ASSET_STATUS_SPEED, 0, 0, 24, 24,
                   start + slot * 24, y - 98, 0);
        ++slot;
    }
    if (rapid) {
        draw_image(ASSET_STATUS_RAPID, 0, 0, 24, 24,
                   start + slot * 24, y - 98, 0);
    }
}

void zombie_host_zombie(int type, int mutation, int x, int y, int frame,
                        int dying, int hit, int direction)
{
    int sx, sy, sw;
    int tint = mutation;
    int asset = ASSET_ZOMBIE;
    int sh = 73;
    if (type >= 3) {
        if (type == 3) {
            asset = ASSET_ANIMAL_CHICKEN; sw = 32; sh = 32;
        } else if (type == 4) {
            asset = ASSET_ANIMAL_PIG; sw = 48; sh = 36;
        } else if (type == 5) {
            asset = ASSET_ANIMAL_SHEEP; sw = 56; sh = 42;
        } else if (type == 6) {
            asset = ASSET_ANIMAL_GOAT; sw = 52; sh = 40;
        } else if (type == 7) {
            asset = ASSET_ANIMAL_COW; sw = 64; sh = 48;
        } else if (type == 8) {
            asset = ASSET_ANIMAL_LLAMA; sw = 60; sh = 44;
        } else if (type == 9) {
            asset = ASSET_ANIMAL_UNICORN; sw = 64; sh = 48;
        } else {
            asset = ASSET_ANIMAL_CROW; sw = 32; sh = 32;
            sx = (frame & 1) * 32; sy = 0;
        }
        if (type != 10) { sx = 0; sy = 0; }
        draw_image_tinted(asset, sx, sy, sw, sh,
                          x - sw / 2, y - sh, direction == 1, tint);
    } else {
        type %= 3;
        if (dying) { sx = (frame % 3) * 84; sy = type * 146 + 73; sw = 84; }
        else { sx = (frame & 1) * 48; sy = type * 146; sw = 48; }
        draw_image_tinted(asset, sx, sy, sw, sh,
                          x - sw / 2, y - sh, direction == 1, tint);
    }
    if (hit) fill_rect(x - 3, y - 46, 6, 6, 0xf800);
}

void zombie_host_pickup(int type, int x, int y, int pulse)
{
    int asset;
    if (type == 0) asset = ASSET_PICKUP_HEALTH;
    else if (type == 1) asset = ASSET_PICKUP_DAMAGE;
    else if (type == 2) asset = ASSET_PICKUP_SPEED;
    else if (type == 3) asset = ASSET_PICKUP_RAPID;
    else asset = ASSET_PICKUP_AMMO;
    if (pulse) fill_rect(x - 14, y - 14, 28, 1, 0xffe0);
    draw_image(asset, 0, 0, 24, 24, x - 12, y - 12, 0);
}

void zombie_host_cow_pie(int x, int y, int pulse)
{
    if (pulse) fill_rect(x - 14, y - 10, 28, 1, 0x07e0);
    draw_image(ASSET_COW_PIE, 0, 0, 24, 16, x - 12, y - 8, 0);
}

void zombie_host_pig(int x, int y, int carried, int hit)
{
    draw_image(ASSET_RESCUE_PIG, 0, 0, 32, 28, x - 16, y - 28, 0);
    if (carried) fill_rect(x - 12, y - 31, 24, 2, 0xffe0);
    if (hit) fill_rect(x - 4, y - 30, 8, 5, 0xf800);
}

void zombie_host_wolf(int x, int y, int frame, int direction, int hit)
{
    draw_image(ASSET_WOLF, (frame & 1) * 64, 0, 64, 48,
               x - 32, y - 48, direction == 0);
    if (hit) fill_rect(x - 5, y - 36, 10, 6, 0xf800);
}

void zombie_host_dog(int x, int y, int frame, int direction,
                     int life, int max_life, int hit)
{
    int width = 0;
    draw_image(ASSET_DOG, (frame & 1) * 64, 0, 64, 40,
               x - 32, y - 40, direction == 0);
    /* A muted, two-pixel bar keeps the companion readable without becoming
     * a second HUD element. */
    fill_rect(x - 24, y - 53, 48, 3, 0x4208);
    if (max_life > 0 && life > 0) {
        width = 48 * life / max_life;
        fill_rect(x - 24, y - 53, width, 2, 0x25a5);
    }
    if (hit) fill_rect(x - 6, y - 31, 12, 6, 0xf800);
}

void zombie_host_bullet(int x, int y, int direction)
{
    /* The original 12x3 sheet contains two 6x3 red bullet frames:
     * right-facing at x=0 and left-facing at x=6.  Render the asset rather
     * than manufacturing direction-dependent colors. */
    draw_image(ASSET_BULLET, direction ? 0 : 6, 0, 6, 3,
               x - 3, y - 46, 0);
}

void zombie_host_hud(int score, int life, int level, int night, int count,
                     int rescue_active, int pigs_saved, int ammo, int ammo_max)
{
    (void)ammo_max;
    fill_rect(0, 0, GAME_W, 39, 0x2104);
    draw_text(6, 4, "ZOMBIE ATTACK", 1);
    draw_text(170, 4, "AMMO", 1); draw_number_scaled(200, 4, ammo, 1);
    draw_text(265, 4, "SCORE", 1); draw_number_scaled(302, 4, score, 1);
    draw_text(395, 4, "LIFE", 1); draw_number_scaled(425, 4, life, 1);
    draw_text(490, 4, "LVL", 1); draw_number_scaled(514, 4, level, 1);
    draw_time_icon(8, 22, night);
    if (rescue_active) {
        draw_text(95, 25, "PIG", 1);
        draw_number(119, 25, pigs_saved);
        draw_text(131, 25, "3", 1);
    } else {
        draw_text(32, 25, night ? "NIGHT" : "DAY", 1);
    }
    draw_text(560, 25, "Z", 1); draw_number(570, 25, count);
}

void zombie_host_launcher(int selected)
{
    static const char *labels[5] = {
        "PLAY CLASSIC GAME", "PLAY ENHANCED GAME", "SETTINGS",
        "HOW TO PLAY", "EXIT GAME"
    };
    int i;
    zombie_host_background(1);
    fill_rect(86, 44, 468, 392, 0x2104);
    fill_rect(91, 49, 458, 382, 0x51e0);
    draw_image(ASSET_ICON, 0, 0, 48, 48, 116, 68, 0);
    draw_text(190, 73, "ZOMBIE ATTACK", 2);
    draw_text(204, 105, "MAIN MENU", 2);
    for (i = 0; i < 5; i++) {
        int y = 145 + i * 45;
        if (i == selected) fill_rect(155, y - 5, 330, 31, 0x4208);
        draw_text(175, y, labels[i], 2);
    }
    draw_text(176, 390, "A SELECT   B BACK", 2);
    draw_text(170, 410, "SELECT START EXIT", 2);
}

void zombie_host_settings_menu(int selected, int sound, int music, int global)
{
    static const char *labels[4] = {
        "SOUND VOLUME", "MUSIC VOLUME", "GLOBAL VOLUME", "BACK"
    };
    int values[3] = { sound, music, global };
    int i;
    zombie_host_background(1);
    fill_rect(96, 65, 448, 350, 0x2104);
    fill_rect(101, 70, 438, 340, 0x51e0);
    draw_text(230, 92, "SETTINGS", 2);
    for (i = 0; i < 4; i++) {
        int y = 150 + i * 45;
        if (i == selected) fill_rect(145, y - 5, 350, 31, 0x4208);
        draw_text(164, y, labels[i], 2);
        if (i < 3) draw_number_scaled(455, y - 1, values[i], 2);
    }
    draw_text(155, 345, "LEFT RIGHT CHANGE", 2);
    draw_text(174, 365, "A B BACK", 2);
    draw_text(145, 390, "MUSIC IS STORED FOR FUTURE", 2);
}

void zombie_host_help(int unused)
{
    (void)unused;
    zombie_host_background(1);
    fill_rect(78, 48, 484, 385, 0x2104);
    fill_rect(83, 53, 474, 375, 0x51e0);
    draw_text(248, 75, "HOW TO PLAY", 2);
    draw_text(130, 120, "D-PAD", 2);
    draw_text(330, 120, "MOVE", 2);
    draw_text(130, 165, "A", 2);
    draw_text(330, 165, "FIRE OR SELECT", 2);
    draw_text(130, 210, "B", 2);
    draw_text(330, 210, "CANCEL MENUS", 2);
    draw_text(130, 255, "START", 2);
    draw_text(330, 255, "PAUSE GAME", 2);
    draw_text(110, 300, "SELECT + START", 2);
    draw_text(330, 300, "EXIT TO FRONTEND", 2);
    draw_text(130, 350, "CARRY PIGLETS TO THE BARN", 1);
    draw_text(130, 365, "SAVE ALL THREE TO HUNT THE WOLF", 1);
    draw_text(254, 395, "A OR B BACK", 2);
}

typedef struct {
    const char *line_one;
    const char *line_two;
} dialogue_t;

static const dialogue_t dialogue_lines[70] = {
    { "", "" },
    { "YOU WALKED IN!", "NOW WALK IT OFF" },
    { "BACK TO THE GRAVE", "NO AUTOGRAPHS" },
    { "FERAL SPEED", "ZERO SURVIVAL" },
    { "FAST MOVE", "SLOW EXIT" },
    { "HEAVY METAL", "LIGHTS OUT" },
    { "TANKS FOR NOTHING", "YOUR ARMOR IS GONE" },
    { "FRIED CHICKEN", "COMING RIGHT UP" },
    { "THAT CHICKEN", "CAME HOME TO DIE" },
    { "PIG DOWN", "BACON THEATER" },
    { "BACON MEETS BULLETS", "BREAKFAST IS READY" },
    { "BAAAAAD TIME!", "YOU ARE DEAD" },
    { "WOOL BE SEEING YOU", "NOT" },
    { "SHEAR DOMINATION", "BYE BYE SHEEP" },
    { "GOAT TO ADMIT", "THAT HURT" },
    { "YOU BLEAT", "THE WRONG COMMANDO" },
    { "MOOOVE ALONG", "NOTHING TO SEE" },
    { "THAT COW", "WAS A BULLSEYE" },
    { "LLAMA TELL YOU", "THAT WAS UGLY" },
    { "BAD HAIR DAY", "WORSE ENDING" },
    { "A UNICORN!", "IN A ZOMBIE FARM" },
    { "RAINBOWS AND RAGE", "WHAT A DAY" },
    { "THAT HORN", "NEEDS TO GO" },
    { "CROW CONTROL", "ENGAGED" },
    { "DARK WINGS", "BAD IDEA" },
    { "BIG BAD WOLF!", "WRONG FARM" },
    { "RED RIDING HOOD", "LEFT THE BUILDING" },
    { "FUR FLYING", "INCOMING" },
    { "BIG BAD MISS", "WOLF DOWN" },
    { "ONE DEAD WOLF", "STAY DOWN" },
    { "GOODBYE WOLF", "RUN ALONG NOW" },
    { "EASY LITTLE PIGGY", "TO THE BARN" },
    { "HANG ON", "BACON EXPRESS" },
    { "NO WOLF", "GETS YOU TODAY" },
    { "SAFE IN THE BARN", "KEEP MOVING" },
    { "PIGGY SECURED", "NICE WORK COMMANDO" },
    { "LET THE CHIPS FALL", "CROWS BEWARE" },
    { "WOOD CHIPS", "ZOMBIE SPLINTERS" },
    { "GROOVY!", "DEADITE LAWN CARE" },
    { "ASH WOULD APPROVE", "MOW THEM DOWN" },
    { "EVIL LAWN", "CLEANUP COMPLETE" },
    { "GET HIM LASSIE!", "ERRR BINGO" },
    { "WHATEVER YOUR NAME", "SIC EM BUDDY" },
    { "MORE BULLETS!", "MORE PROBLEMS" },
    { "AMMO ACQUIRED", "KEEP FIRING" },
    { "TURN UP THE HEAT", "ZOMBIES MELT" },
    { "DAMAGE BOOST", "NOW WE TALK" },
    { "FREE HEALTH", "THANKS FARM" },
    { "PATCHED UP", "BACK TO WORK" },
    { "GOTTA GO FAST", "ZOMBIES HATE IT" },
    { "SPEED BOOST", "CATCH ME NOW" },
    { "RAPID FIRE", "LOUD AND PROUD" },
    { "MORE LEAD", "LESS DEAD" },
    { "SQUIRREL SIGHTED", "BAD NUTS INCOMING" },
    { "NUTS TO YOU", "KEEP YOUR HEAD DOWN" },
    { "DON'T HAVE A COW!", "WAIT..." },
    { "THAT COW", "IS HAVING A BAD DAY" },
    { "STOP CHICKEN ME OUT...", "IT'S CREEPY" },
    { "THAT CHICKEN", "IS NO JOKE" },
    { "THE WRONG MOVIE", "FOR YOU WALKER" },
    { "RUNNING ON EMPTY", "LIKE YOUR FUTURE" },
    { "BIG GUY", "BIGGER TARGET" },
    { "BAD POULTRY", "GOOD AIM" },
    { "HAMMER TIME", "NO HAM REQUIRED" },
    { "LAMB TO SLAUGHTER", "CUT TO CREDITS" },
    { "GOATTA GO", "THE EXIT IS CLOSED" },
    { "FARM TO TABLE", "TABLE FOR ONE" },
    { "NO PROB-LLAMA", "PROBLEM SOLVED" },
    { "YOUR RAINBOW", "HAS BEEN CANCELED" },
    { "BIRD BRAINS", "MEET BULLETS" }
};

void zombie_host_dialogue(int id, int x, int y)
{
    int width;
    int line_width;
    int left;
    int top;
    if (id <= 0 || id >= (int)(sizeof(dialogue_lines) / sizeof(dialogue_lines[0])))
        return;
    line_width = (int)strlen(dialogue_lines[id].line_one);
    width = (int)strlen(dialogue_lines[id].line_two);
    if (width > line_width) line_width = width;
    width = line_width * 6 + 20;
    if (width < 150) width = 150;
    if (width > 330) width = 330;
    left = x - width / 2;
    if (left < 5) left = 5;
    if (left + width > GAME_W - 5) left = GAME_W - 5 - width;
    top = y - 145;
    if (top < 42) top = 42;
    fill_rect(left, top, width, 38, 0x0000);
    fill_rect(left + 2, top + 2, width - 4, 34, 0xffe0);
    draw_text_color(left + 10, top + 7, dialogue_lines[id].line_one, 1, 0x0000);
    draw_text_color(left + 10, top + 20, dialogue_lines[id].line_two, 1, 0x0000);
    fill_rect(x - 5, top + 38, 10, 3, 0x0000);
    fill_rect(x - 2, top + 41, 4, 3, 0x0000);
}

void zombie_host_upgrade(int selected, unsigned int flags, unsigned int ranks,
                         int enhanced)
{
    static const char *enhanced_labels[10] = {
        "FAST WALK", "MORE SHOTS", "FAST SHOOT", "LASER AIM",
        "WOOD CHIPPER", "TIME STOP", "MORE LIFE", "BEAGLE BUDDY",
        "MORE DAMAGE", "LAWN MOWER"
    };
    static const char *classic_labels[10] = {
        "FAST WALK", "MORE SHOTS", "FAST SHOOT", "LASER AIM",
        "MORE DIRECTIONS", "TIME STOP", "MORE LIFE", "FAST LIFE",
        "MORE DAMAGE", "GRENADES"
    };
    static const unsigned int codes[10] = {
        2u, 4u, 8u, 16u, 32u, 64u, 128u, 256u, 512u, 1u
    };
    static const int enhanced_max_ranks[10] = { 5, 5, 5, 5, 5,
                                                5, 5, 5, 5, 5 };
    int i;
    fill_rect(35, 102, 570, 275, 0x7820);
    fill_rect(40, 107, 560, 265, 0x51e0);
    draw_text(230, 115, "UPGRADES", 2);
    for (i = 0; i < 10; i++) {
        int col = i >= 5;
        int row = i % 5;
        int x = col ? 330 : 55;
        int y = 150 + row * 31;
        int rank = (int)((ranks >> (i * 3)) & 7u);
        int max_rank = enhanced ? enhanced_max_ranks[i] : 1;
        int owned = rank >= max_rank ||
                    (max_rank == 1 && (flags & codes[i]) != 0);
        if (i == selected) {
            fill_rect(x - 3, y - 3, 256, 35, owned ? 0xf800 : 0x4208);
        }
        fill_rect(x, y, 250, 29, owned ? 0x7bef : 0x8c41);
        draw_text(x + 8, y + 7, enhanced ? enhanced_labels[i] : classic_labels[i], 2);
        draw_text(x + 195, y + 8, "R", 1);
        draw_number(x + 204, y + 8, rank);
        draw_text(x + 216, y + 8, "M", 1);
        draw_number(x + 225, y + 8, max_rank);
    }
    draw_text(175, 345, "A SELECT", 2);
    draw_text(295, 345, "B START CANCEL", 1);
}

void zombie_host_mower(int x, int y, int direction)
{
    draw_image(ASSET_MOWER, 0, 0, 64, 32, x - 32, y - 32, direction == 0);
    fill_rect(x - 34, y - 3, 68, 2, 0x07e0);
}

void zombie_host_chipper(int x, int y, int direction)
{
    draw_image(ASSET_CHIPPER, 0, 0, 64, 32, x - 32, y - 32, direction == 0);
    fill_rect(direction ? x + 26 : x - 90, y - 27, 64, 1, 0xffe0);
}

void zombie_host_squirrel(int x, int y, int frame, int direction)
{
    draw_image(ASSET_SQUIRREL, (frame & 1) * 48, 0, 48, 40,
               x - 24, y - 40, direction == 0);
}

void zombie_host_bad_nut(int x, int y, int pulse)
{
    if (pulse) fill_rect(x - 10, y - 10, 20, 1, 0x87e0);
    draw_image(ASSET_BAD_NUT, 0, 0, 16, 16, x - 8, y - 8, 0);
}

void zombie_host_pause(int unused)
{
    (void)unused;
    fill_rect(220, 185, 200, 95, 0x0000);
    draw_text(260, 205, "PAUSED", 2);
    draw_text(248, 245, "START RESUME", 2);
}

void zombie_host_game_over(int score)
{
    fill_rect(160, 190, 320, 90, 0x0000);
    draw_text(230, 210, "GAME OVER", 2);
    draw_text(220, 245, "SCORE", 2); draw_number_scaled(280, 241, score, 2);
}

void zombie_host_present(int unused)
{
    (void)unused;
    if (display_active) hwdisp_present(framebuffer, GAME_W, GAME_H, GAME_W * 2);
}

static int tone_frames[4];
static unsigned int tone_phase[4];
static unsigned int tone_step[4];

static void audio_tone(int id)
{
    static const unsigned int steps[4] = { 382293, 510417, 680891, 255208 };
    int slot = id & 3;
    tone_frames[slot] = id == 0 ? 2400 : 7200;
    tone_phase[slot] = 0;
    tone_step[slot] = steps[slot];
}

void zombie_host_sound(int id)
{
    if (audio_active && sound_volume > 0 && global_volume > 0) audio_tone(id);
}

void zombie_host_audio_pump(int unused)
{
    short buffer[1600];
    int i, j, sample;
    (void)unused;
    if (!audio_active) return;
    for (i = 0; i < 800; i++) {
        sample = 0;
        for (j = 0; j < 4; j++) {
            if (tone_frames[j] > 0) {
                sample += (tone_phase[j] & 0x80000000u) ? 900 : -900;
                tone_phase[j] += tone_step[j];
                tone_frames[j]--;
            }
        }
        sample = sample * sound_volume * global_volume / 25;
        if (sample > 3000) sample = 3000;
        if (sample < -3000) sample = -3000;
        buffer[i * 2] = (short)sample;
        buffer[i * 2 + 1] = (short)sample;
    }
    hwaudio_write(buffer, 800);
}
