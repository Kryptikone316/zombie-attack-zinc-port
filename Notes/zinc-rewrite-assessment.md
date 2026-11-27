# Zombie Attack Zinc Rewrite Assessment

## Source identification

The supplied project is an Eclipse CDT C project named `AtakZombie`, not a
Java project. The authoritative implementation is under:

`Installer/ZombieAttack/Source/`

Evidence:

- `.project` declares Eclipse CDT C natures and a managed GCC build.
- `.cproject` uses GCC, `-m32`, and SDL.
- `m_main.c` includes SDL, SDL_image, and gettext, and creates a 640x480
  8-bit SDL surface.
- `zombieattack.spec` requires SDL >= 1.2.
- The source and assets are GPLv3-distributed; preserve `COPYING` and source
  attribution in any derivative port.

The Eclipse requirement is historical project workflow guidance. It is not a
runtime or language requirement.

## Game scope

The original is a compact top-down survival game:

- 640x480 8-bit farm battlefield;
- player movement, shooting, weapon heat, life, score, levels, and upgrades;
- zombie spawning and pursuit with a maximum of 128 active zombies;
- up to 64 active bullets;
- day/night environment changes and ambient/zombie audio;
- ten upgrade choices, including movement, firing, life, damage, grenade,
  laser aim, and time stop;
- pause and escape-to-exit behavior.

The main loop uses elapsed milliseconds, integer state, and simple collision/
movement rules. This is a good fit for Zinc and the SF3500's limited MIPS
hardware.

## Assets

The original assets are unusually suitable for offline conversion:

- BMP sprites/backgrounds are 1-, 4-, or 8-bit palette images;
- WAV files are unsigned 8-bit, mono, 11025 Hz;
- sprite sheets contain the player and zombie animation frames;
- the supplied `Data/pack.sh` optionally gzip-compresses BMP/WAV assets.

The Zinc port should convert these assets at build time into simple raw
palette/sprite and PCM formats. A runtime BMP, PNG, gzip, SDL, or gettext
dependency should not be required.

## Zinc port boundary

The game rules, state machines, timers, sprite selection, collision logic,
menus, and software renderer can be implemented in Zinc. A small C ABI shim
will still be appropriate for the platform boundary, as in the Zinc tests:

- SF3500 display/framebuffer setup and frame presentation;
- `joykey` input and Select+Start emergency exit;
- monotonic timing;
- H.OS audio initialization and PCM playback/mixing;
- raw file loading where Zinc's existing file library is insufficient.

This keeps the game implementation Zinc-native without attempting to expose
SDL 1.2 on H.OS.

## Main risks

1. The original assumes a 640x480 landscape SDL surface, while SF3500 reports
   an 854x480 panel with rotation. The first playable slice must establish the
   logical viewport, scaling, and rotation policy.
2. The original depends on SDL palette/color-key blits and dynamically
   generated font surfaces. Zinc needs a small software blitter and font
   renderer for the converted assets.
3. The original audio mixer expects concurrent 11025 Hz U8 channels. H.OS
   audio must be tested before committing to exact simultaneous mixing.
4. Original keyboard controls need an explicit handheld mapping.

## Recommended implementation order

1. Preserve and document the original behavior and GPLv3 materials.
2. Add a Zinc SF3500 runtime shim and a 640x480 logical framebuffer test.
3. Convert and display the farm, player, zombie, bullet, and font assets.
4. Port the fixed-timestep game state and player controls.
5. Add zombie AI, bullets, damage, score, upgrades, and day/night behavior.
6. Add audio, pause, emergency exit, packaging, and hardware verification.

No gameplay source has been modified by this assessment.

## Initial playable build

The first Zinc implementation is under `zinc/` and has been built for
MIPS32r2/o32 hard-float and deployed to the mounted SF3500 card as:

- `/roms/ports/Zombie Attack.sh`;
- `/roms/ports/zombieattack/zombie-attack.mipsel`;
- `/roms/ports/zombieattack/data/`.

The build was smoke-tested natively for asset loading and loop entry. Actual
display, input, audio, performance, and emergency-exit behavior remain to be
verified on the SF3500. This first hardware build is intentionally a playable
baseline: it uses the original farm/player/zombie artwork and the H.OS audio
boundary, while its audio is currently generated test tones and its upgrade
menu covers the initial core upgrade set. Exact original audio mixing and all
ten original upgrades remain follow-up fidelity work after hardware behavior
is confirmed.

### Follow-up correction build

After hardware play testing, the following baseline defects were corrected:

- bullets now use the original red `bullet.bmp` frames in both directions;
- projectile collision uses the player/zombie battlefield Y coordinate rather
  than the bullet's visual offset, restoring line-of-sight hits;
- Start no longer opens upgrades for free; the menu is entered only when the
  score reaches the next-level threshold;
- sprite drawing now follows the original foreground/background Y ordering.

The upgrade menu now follows the original ten-option layout and ownership
flags. Already selected upgrades are visibly disabled, and B or Start cancels
the menu without consuming the pending level offer; Start can reopen that
offer later.

This correction build was rebuilt, smoke-tested for asset loading and loop
entry, and hash-verified onto the same mounted card. Hardware confirmation of
the sprite appearance and collision behavior remains pending.
