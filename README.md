# Zombie Attack — Zinc MIPS Port

This repository contains the Zinc rewrite of the original Zombie Attack game
for H.OS MIPS handhelds, including the SF3500 target.

- Zinc game logic: [`zinc/src/zombie_game.zc`](zinc/src/zombie_game.zc)
- SF3500/H.OS host boundary: [`zinc/src/zombie_host.c`](zinc/src/zombie_host.c)
- Enhanced assets: [`zinc/assets/`](zinc/assets/)
- Build/deployment package: [`zinc/build_mips.sh`](zinc/build_mips.sh)
- Mode and feature history: [`Notes/original-vs-modes.md`](Notes/original-vs-modes.md)

Build with the accessible SF3500 Buildroot toolchain:

```sh
./zinc/build_mips.sh
```

The generated PortMaster-style package is written to `zinc/dist/roms/ports/`.
The original reference source and data remain under `Installer/ZombieAttack/`.
