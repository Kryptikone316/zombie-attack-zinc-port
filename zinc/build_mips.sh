#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
ZINC_ROOT=${ZINC_ROOT:-/home/kryptik/handhelds/Development/Zinc/zinc-1.3}
TC_ROOT=${TC_ROOT:-/home/kryptik/handhelds/sf3000-work/buildroot/output/host}
MIPS_PREFIX=${MIPS_PREFIX:-$TC_ROOT/bin/mips-mti-linux-gnu-}
SYSROOT=${SYSROOT:-$TC_ROOT/mipsel-buildroot-linux-gnu/sysroot}
HW_ROOT=${HW_ROOT:-/home/kryptik/handhelds/sf3000-work/Atomiks/src/hw}
ORIGINAL_DATA=$ROOT/../Installer/ZombieAttack/Data
ENHANCED_DATA=$ROOT/assets
BUILD=$ROOT/build
DIST=$ROOT/dist/roms/ports

mkdir -p "$BUILD" "$DIST/zombieattack/data"

gcc -m32 -O2 -fomit-frame-pointer \
    "$ZINC_ROOT/bootstrap/zc.c" "$ZINC_ROOT/bootstrap/io.c" \
    -o "$BUILD/zc-host"

"$BUILD/zc-host" \
    -I "$ZINC_ROOT/lib/platform/h.os-mips32r2-o32" \
    -I "$ZINC_ROOT/lib" \
    -I "$ROOT/src" \
    -o "$BUILD/zombie_game.c" "$ROOT/src/zombie_game.zc"

MIPS_FLAGS="--sysroot=$SYSROOT -EL -mips32r2 -mtune=74kc -mabi=32 -mhard-float -mfp32 -mnan=legacy -G0 -O2 -fno-strict-aliasing -Wall"

"${MIPS_PREFIX}gcc" $MIPS_FLAGS -I "$ROOT/src" -I "$HW_ROOT" \
    -c "$BUILD/zombie_game.c" -o "$BUILD/zombie_game.o"
"${MIPS_PREFIX}gcc" $MIPS_FLAGS -I "$ROOT/src" -I "$HW_ROOT" \
    -c "$ROOT/src/zombie_host.c" -o "$BUILD/zombie_host.o"
"${MIPS_PREFIX}gcc" $MIPS_FLAGS -I "$HW_ROOT" \
    -c "$HW_ROOT/hwdisp.c" -o "$BUILD/hwdisp.o"
"${MIPS_PREFIX}gcc" $MIPS_FLAGS -I "$HW_ROOT" \
    -c "$HW_ROOT/hwaudio.c" -o "$BUILD/hwaudio.o"
"${MIPS_PREFIX}gcc" $MIPS_FLAGS -I "$HW_ROOT" \
    -c "$HW_ROOT/joykey.c" -o "$BUILD/joykey.o"

"${MIPS_PREFIX}gcc" $MIPS_FLAGS -o "$BUILD/zombie-attack.mipsel" \
    "$BUILD/zombie_game.o" "$BUILD/zombie_host.o" "$BUILD/hwdisp.o" \
    "$BUILD/hwaudio.o" "$BUILD/joykey.o" -ldl -lpthread

cp "$BUILD/zombie-attack.mipsel" "$DIST/zombieattack/"
cp "$ROOT/run.sh" "$ROOT/README.md" "$DIST/zombieattack/"
cp "$ROOT/Zombie Attack.sh" "$DIST/"
find "$ORIGINAL_DATA" -maxdepth 1 -type f -exec cp -a {} "$DIST/zombieattack/data/" \;
find "$ENHANCED_DATA" -maxdepth 1 -type f -exec cp -a {} "$DIST/zombieattack/data/" \;
chmod 755 "$DIST/zombieattack/run.sh" "$DIST/Zombie Attack.sh"

echo "built $DIST/zombieattack/zombie-attack.mipsel"
