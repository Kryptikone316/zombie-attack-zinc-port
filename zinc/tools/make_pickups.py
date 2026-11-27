#!/usr/bin/env python3
"""Create Enhanced-mode pickup, status, and visibly undead farm sprites."""

from pathlib import Path
from PIL import Image, ImageDraw

OUT = Path(__file__).resolve().parents[1] / "assets"
MAGENTA = (255, 0, 255)


def save(name, draw_icon, size=(24, 24)):
    image = Image.new("P", size, 0)
    palette = [MAGENTA, (255, 255, 255), (180, 35, 35), (255, 255, 255),
               (145, 85, 30), (225, 185, 55), (20, 20, 25), (55, 125, 45),
               (245, 150, 165), (120, 120, 120), (245, 225, 170),
               (75, 40, 20), (80, 150, 220), (185, 150, 225)]
    image.putpalette([component for color in palette for component in color] + [0] * (768 - len(palette) * 3))
    draw = ImageDraw.Draw(image)
    draw_icon(draw)
    image.save(OUT / name, format="BMP")


def health(draw):
    # Red barn with a white first-aid cross.
    draw.rectangle((2, 9, 21, 21), fill=6)
    draw.polygon([(1, 10), (12, 2), (23, 10)], fill=2)
    draw.rectangle((5, 10, 19, 21), fill=2)
    draw.rectangle((10, 14, 14, 21), fill=4)
    draw.rectangle((9, 11, 15, 17), fill=3)
    draw.rectangle((11, 10, 13, 18), fill=3)
    draw.rectangle((8, 13, 16, 15), fill=3)


def damage(draw):
    # A pitchfork: a farm tool that also communicates increased attack power.
    draw.line((11, 21, 12, 7), fill=4, width=3)
    draw.line((12, 7, 5, 3), fill=6, width=2)
    draw.line((12, 7, 12, 2), fill=6, width=2)
    draw.line((12, 7, 19, 3), fill=6, width=2)
    draw.line((5, 3, 5, 7), fill=6, width=2)
    draw.line((12, 2, 12, 6), fill=6, width=2)
    draw.line((19, 3, 19, 7), fill=6, width=2)
    draw.rectangle((8, 18, 15, 22), fill=5)


def speed(draw):
    # Green work boot with motion arrows for the temporary speed boost.
    draw.polygon([(3, 4), (9, 4), (11, 13), (18, 16), (21, 21),
                  (5, 21), (2, 17)], fill=7)
    draw.line((14, 5, 21, 5), fill=12, width=2)
    draw.line((17, 2, 21, 5), fill=12, width=2)
    draw.line((17, 8, 23, 8), fill=12, width=2)
    draw.line((19, 6, 23, 8), fill=12, width=2)


def rapid(draw):
    # Gold lightning and paired rounds for the temporary rapid-fire boost.
    draw.polygon([(12, 1), (4, 13), (10, 13), (7, 23),
                  (20, 9), (14, 9)], fill=5)
    draw.line((17, 16, 23, 16), fill=2, width=2)
    draw.line((16, 21, 22, 21), fill=2, width=2)


def ammo(draw):
    # Wooden ammunition crate with two readable brass rounds.
    draw.rectangle((2, 5, 21, 21), fill=4)
    draw.rectangle((4, 7, 19, 19), outline=11, width=2)
    draw.line((4, 12, 19, 12), fill=11, width=2)
    draw.rectangle((7, 2, 10, 10), fill=5)
    draw.rectangle((14, 2, 17, 10), fill=5)
    draw.rectangle((8, 3, 9, 5), fill=3)
    draw.rectangle((15, 3, 16, 5), fill=3)


def cow_pie(draw):
    # Clearly readable farm hazard: a brown dung pile with green flies.
    draw.ellipse((2, 7, 21, 15), fill=11)
    draw.ellipse((5, 4, 16, 12), fill=4)
    draw.ellipse((9, 2, 19, 10), fill=11)
    draw.line((3, 13, 21, 13), fill=6, width=2)
    draw.point((5, 1), fill=7)
    draw.point((19, 3), fill=7)


def status_damage(draw):
    # Compact gold pitchfork status marker.
    draw.line((8, 14, 8, 4), fill=5, width=2)
    draw.line((8, 5, 3, 2), fill=6, width=1)
    draw.line((8, 5, 8, 1), fill=6, width=1)
    draw.line((8, 5, 13, 2), fill=6, width=1)


def status_poison(draw):
    # Green diseased droplet with a dark outline.
    draw.polygon([(8, 1), (3, 8), (3, 12), (5, 15), (11, 15), (13, 12),
                  (13, 8)], fill=6)
    draw.polygon([(8, 3), (5, 9), (5, 12), (7, 14), (10, 14), (12, 11),
                  (11, 8)], fill=7)
    draw.point((7, 9), fill=1)
    draw.point((10, 9), fill=1)


def status_cripple(draw):
    # Broken boot/leg marker for the movement penalty.
    draw.line((5, 2, 7, 9), fill=4, width=3)
    draw.line((7, 9, 12, 12), fill=4, width=3)
    draw.line((3, 13, 12, 13), fill=6, width=2)
    draw.line((7, 9, 10, 7), fill=2, width=1)


def status_speed(draw):
    # Small green boot status marker.
    draw.polygon([(3, 5), (8, 5), (10, 13), (16, 15), (17, 20),
                  (4, 20), (2, 16)], fill=7)
    draw.line((16, 5, 22, 5), fill=12, width=2)
    draw.line((19, 2, 22, 5), fill=12, width=2)


def status_rapid(draw):
    # Two red rounds with a small lightning accent.
    draw.line((3, 7, 14, 7), fill=2, width=2)
    draw.line((3, 14, 14, 14), fill=2, width=2)
    draw.polygon([(18, 1), (13, 11), (17, 11), (15, 20),
                  (23, 9), (19, 9)], fill=5)


def crow_frame(draw, offset, wings_up):
    # Dark undead crow with a red eye and sickly green wing highlights.
    draw.ellipse((10 + offset, 12, 23 + offset, 26), fill=6)
    draw.ellipse((15 + offset, 7, 26 + offset, 18), fill=6)
    draw.polygon([(24 + offset, 11), (31 + offset, 14),
                  (24 + offset, 16)], fill=11)
    draw.point((22 + offset, 11), fill=2)
    if wings_up:
        draw.polygon([(14 + offset, 16), (4 + offset, 3),
                      (11 + offset, 5), (18 + offset, 14)], fill=9)
        draw.polygon([(19 + offset, 17), (28 + offset, 3),
                      (29 + offset, 10), (23 + offset, 20)], fill=7)
    else:
        draw.polygon([(13 + offset, 17), (2 + offset, 25),
                      (5 + offset, 29), (18 + offset, 22)], fill=9)
        draw.polygon([(20 + offset, 18), (29 + offset, 26),
                      (27 + offset, 30), (18 + offset, 23)], fill=7)
    draw.line((12 + offset, 24, 9 + offset, 30), fill=11, width=1)
    draw.line((19 + offset, 24, 21 + offset, 30), fill=11, width=1)


def animal_crow(draw):
    crow_frame(draw, 0, True)
    crow_frame(draw, 32, False)


def piglet(draw):
    # Friendly rescue piglet: deliberately distinct from the undead pig enemy.
    draw.ellipse((3, 11, 24, 25), fill=8)
    draw.ellipse((14, 7, 30, 21), fill=8)
    draw.polygon([(16, 8), (14, 2), (21, 7)], fill=8)
    draw.polygon([(25, 8), (30, 3), (30, 12)], fill=8)
    draw.ellipse((23, 14, 31, 21), fill=10)
    draw.point((25, 17), fill=6)
    draw.point((29, 17), fill=6)
    draw.point((21, 12), fill=6)
    draw.line((8, 23, 7, 28), fill=6, width=2)
    draw.line((20, 23, 21, 28), fill=6, width=2)


def wolf_frame(draw, offset, stride):
    # Storybook gray wolf, not undead: red scarf, teeth, and a hungry eye.
    draw.ellipse((8 + offset, 21, 45 + offset, 39), fill=9)
    draw.ellipse((35 + offset, 10, 58 + offset, 30), fill=9)
    draw.polygon([(38 + offset, 12), (38 + offset, 1),
                  (46 + offset, 10)], fill=6)
    draw.polygon([(51 + offset, 12), (58 + offset, 2),
                  (60 + offset, 18)], fill=6)
    draw.polygon([(54 + offset, 19), (63 + offset, 22),
                  (54 + offset, 25)], fill=11)
    draw.point((50 + offset, 17), fill=2)
    draw.polygon([(35 + offset, 25), (22 + offset, 16),
                  (12 + offset, 23), (29 + offset, 31)], fill=2)
    draw.polygon([(8 + offset, 26), (1 + offset, 17),
                  (4 + offset, 31), (13 + offset, 35)], fill=9)
    draw.line((17 + offset, 35, 14 + offset, 47), fill=6, width=3)
    draw.line((38 + offset, 35, 40 + offset, 47), fill=6, width=3)
    if stride:
        draw.line((27 + offset, 35, 24 + offset, 46), fill=6, width=3)
    else:
        draw.line((27 + offset, 35, 30 + offset, 46), fill=6, width=3)


def animal_wolf(draw):
    wolf_frame(draw, 0, False)
    wolf_frame(draw, 64, True)


def dog_frame(draw, offset, stride):
    # Friendly tricolor beagle companion with a white muzzle and chest.
    draw.ellipse((7 + offset, 16, 43 + offset, 34), fill=10)
    draw.ellipse((34 + offset, 9, 59 + offset, 29), fill=10)
    draw.ellipse((42 + offset, 17, 61 + offset, 29), fill=1)
    draw.ellipse((37 + offset, 8, 47 + offset, 18), fill=11)
    draw.polygon([(38 + offset, 11), (31 + offset, 3),
                  (42 + offset, 7)], fill=11)
    draw.point((50 + offset, 16), fill=6)
    draw.point((57 + offset, 21), fill=6)
    draw.polygon([(9 + offset, 17), (1 + offset, 7),
                  (5 + offset, 25), (14 + offset, 27)], fill=11)
    draw.line((16 + offset, 31, 14 + offset, 39), fill=6, width=3)
    draw.line((38 + offset, 31, 40 + offset, 39), fill=6, width=3)
    if stride:
        draw.line((27 + offset, 31, 24 + offset, 39), fill=6, width=3)
    else:
        draw.line((27 + offset, 31, 30 + offset, 39), fill=6, width=3)


def animal_dog(draw):
    dog_frame(draw, 0, False)
    dog_frame(draw, 64, True)


def mower(draw):
    # Compact push mower arriving from the barn.
    draw.rectangle((8, 18, 42, 27), fill=6)
    draw.rectangle((14, 13, 38, 21), fill=2)
    draw.line((36, 14, 54, 3), fill=11, width=3)
    draw.line((51, 3, 60, 3), fill=11, width=3)
    draw.ellipse((11, 24, 19, 32), fill=6)
    draw.ellipse((37, 24, 45, 32), fill=6)


def chipper(draw):
    # Farm wood-chipper: a hopper and a spray of pale wood chips.
    draw.rectangle((7, 16, 38, 28), fill=4)
    draw.polygon([(10, 16), (18, 4), (34, 4), (39, 16)], fill=5)
    draw.rectangle((18, 20, 28, 25), fill=6)
    draw.line((38, 9, 49, 5), fill=10, width=2)
    draw.line((39, 12, 55, 12), fill=10, width=2)
    draw.line((39, 15, 50, 20), fill=10, width=2)


def squirrel_frame(draw, offset, facing):
    # Neutral squirrel peeking from the tree hole; the red eye is a playful
    # accent, not an undead mutation marker.
    draw.ellipse((8 + offset, 14, 34 + offset, 31), fill=11)
    draw.ellipse((25 + offset, 8, 46 + offset, 25), fill=4)
    draw.polygon([(27 + offset, 10), (29 + offset, 1),
                  (36 + offset, 9)], fill=4)
    draw.polygon([(38 + offset, 10), (44 + offset, 2),
                  (47 + offset, 13)], fill=4)
    draw.polygon([(43 + offset, 17), (49 + offset, 19),
                  (43 + offset, 22)], fill=10)
    draw.point((39 + offset, 14), fill=2)
    draw.line((15 + offset, 26, 11 + offset, 37), fill=6, width=3)
    draw.line((29 + offset, 27, 31 + offset, 37), fill=6, width=3)
    if facing:
        draw.ellipse((0 + offset, 4, 18 + offset, 24), fill=4)
    else:
        draw.ellipse((35 + offset, 4, 47 + offset, 24), fill=4)


def squirrel(draw):
    squirrel_frame(draw, 0, True)
    squirrel_frame(draw, 48, False)


def bad_nut(draw):
    # A cracked, sickly nut: small and readable without looking like a pickup.
    draw.ellipse((2, 3, 13, 14), fill=4)
    draw.line((8, 3, 6, 8), fill=2, width=1)
    draw.line((7, 8, 11, 13), fill=2, width=1)
    draw.point((3, 5), fill=7)


def animal_chicken(draw):
    # Undead chicken: fast nuisance enemy with sickly plumage and a wound.
    draw.ellipse((5, 10, 27, 27), fill=7)
    draw.ellipse((8, 7, 20, 18), fill=9)
    draw.rectangle((12, 5, 15, 8), fill=2)
    draw.rectangle((15, 10, 18, 13), fill=6)
    draw.point((17, 11), fill=2)
    draw.polygon([(8, 14), (2, 12), (8, 18)], fill=5)
    draw.line((21, 18, 25, 22), fill=2, width=2)
    draw.line((11, 26, 9, 31), fill=4, width=2)
    draw.line((19, 26, 21, 31), fill=4, width=2)


def animal_pig(draw):
    # Undead pig bruiser with diseased hide, sockets, and a torn flank.
    draw.ellipse((4, 8, 42, 31), fill=6)
    draw.ellipse((8, 10, 38, 28), fill=7)
    draw.polygon([(8, 10), (5, 2), (15, 8)], fill=7)
    draw.polygon([(30, 8), (39, 2), (37, 13)], fill=7)
    draw.ellipse((28, 19, 40, 28), fill=11)
    draw.point((32, 23), fill=6)
    draw.point((37, 23), fill=6)
    draw.point((18, 16), fill=2)
    draw.point((29, 16), fill=2)
    draw.line((11, 19, 16, 23), fill=2, width=2)
    draw.line((11, 28, 10, 35), fill=6, width=3)
    draw.line((33, 28, 35, 35), fill=6, width=3)


def animal_cow(draw):
    # Undead broad cow/tank silhouette with horns, spots, and red eyes.
    draw.ellipse((5, 13, 57, 42), fill=6)
    draw.ellipse((10, 10, 50, 38), fill=7)
    draw.ellipse((14, 14, 25, 25), fill=11)
    draw.ellipse((37, 23, 49, 34), fill=11)
    draw.ellipse((42, 30, 57, 41), fill=9)
    draw.polygon([(12, 12), (3, 3), (16, 8)], fill=6)
    draw.polygon([(46, 11), (57, 3), (49, 16)], fill=6)
    draw.point((30, 17), fill=2)
    draw.point((39, 17), fill=2)
    draw.line((20, 28, 28, 34), fill=2, width=2)
    draw.line((16, 37, 14, 46), fill=6, width=4)
    draw.line((45, 37, 48, 46), fill=6, width=4)


def animal_sheep(draw):
    # Undead fluffy bruiser: gray wool, dark face, red eyes, and a wound.
    draw.ellipse((3, 13, 51, 38), fill=9)
    draw.ellipse((9, 9, 22, 22), fill=9)
    draw.ellipse((18, 7, 31, 21), fill=9)
    draw.ellipse((28, 9, 42, 23), fill=9)
    draw.ellipse((39, 14, 54, 31), fill=6)
    draw.rectangle((43, 20, 54, 31), fill=9)
    draw.point((47, 20), fill=2)
    draw.point((51, 20), fill=2)
    draw.line((27, 24, 33, 30), fill=2, width=2)
    draw.line((13, 34, 11, 41), fill=6, width=3)
    draw.line((38, 34, 40, 41), fill=6, width=3)


def animal_goat(draw):
    # Undead lean, horned bruiser; angular and faster than the sheep.
    draw.ellipse((4, 12, 45, 34), fill=7)
    draw.ellipse((26, 8, 49, 28), fill=9)
    draw.polygon([(31, 10), (27, 1), (35, 7)], fill=6)
    draw.polygon([(43, 9), (50, 1), (48, 15)], fill=6)
    draw.rectangle((40, 20, 51, 29), fill=4)
    draw.point((43, 17), fill=2)
    draw.point((48, 17), fill=2)
    draw.line((15, 18, 21, 23), fill=2, width=2)
    draw.line((12, 30, 9, 39), fill=6, width=3)
    draw.line((34, 30, 36, 39), fill=6, width=3)
    draw.line((23, 30, 21, 39), fill=6, width=3)


def animal_llama(draw):
    # Uncommon undead llama: longer neck, ears, sockets, and torn hide.
    draw.ellipse((4, 22, 48, 40), fill=7)
    draw.rectangle((28, 7, 42, 30), fill=9)
    draw.ellipse((32, 4, 50, 18), fill=6)
    draw.polygon([(34, 6), (34, 0), (40, 6)], fill=4)
    draw.polygon([(44, 7), (48, 1), (50, 12)], fill=4)
    draw.point((42, 10), fill=2)
    draw.point((47, 10), fill=2)
    draw.line((17, 27, 23, 32), fill=2, width=2)
    draw.line((12, 36, 10, 43), fill=6, width=3)
    draw.line((38, 36, 40, 43), fill=6, width=3)
    draw.line((27, 28, 27, 43), fill=6, width=3)


def animal_unicorn(draw):
    # Exceedingly rare undead elite: pale sickly body, sockets, and horn.
    draw.ellipse((3, 18, 57, 43), fill=9)
    draw.rectangle((34, 8, 51, 29), fill=7)
    draw.ellipse((40, 5, 60, 22), fill=6)
    draw.polygon([(45, 8), (51, 0), (53, 10)], fill=5)
    draw.rectangle((38, 11, 43, 15), fill=12)
    draw.point((50, 12), fill=2)
    draw.point((56, 12), fill=2)
    draw.line((18, 26, 26, 34), fill=2, width=2)
    draw.line((12, 38, 10, 47), fill=6, width=3)
    draw.line((46, 38, 48, 47), fill=6, width=3)
    draw.line((27, 36, 26, 47), fill=6, width=3)


OUT.mkdir(parents=True, exist_ok=True)
save("pickup-health.bmp", health)
save("pickup-damage.bmp", damage)
save("pickup-speed.bmp", speed)
save("pickup-rapid.bmp", rapid)
save("pickup-ammo.bmp", ammo)
save("status-damage.bmp", status_damage)
save("status-poison.bmp", status_poison)
save("status-cripple.bmp", status_cripple)
save("status-speed.bmp", status_speed)
save("status-rapid.bmp", status_rapid)
save("cow-pie.bmp", cow_pie, (24, 16))
save("animal-chicken.bmp", animal_chicken, (32, 32))
save("animal-pig.bmp", animal_pig, (48, 36))
save("animal-sheep.bmp", animal_sheep, (56, 42))
save("animal-goat.bmp", animal_goat, (52, 40))
save("animal-cow.bmp", animal_cow, (64, 48))
save("animal-llama.bmp", animal_llama, (60, 48))
save("animal-unicorn.bmp", animal_unicorn, (64, 48))
save("animal-crow.bmp", animal_crow, (64, 32))
save("rescue-piglet.bmp", piglet, (32, 28))
save("big-bad-wolf.bmp", animal_wolf, (128, 48))
save("beagle-buddy.bmp", animal_dog, (128, 40))
save("lawn-mower.bmp", mower, (64, 32))
save("wood-chipper.bmp", chipper, (64, 32))
save("squirrel.bmp", squirrel, (96, 40))
save("bad-nut.bmp", bad_nut, (16, 16))
