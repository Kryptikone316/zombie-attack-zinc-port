#!/bin/sh
set -eu
PORT_DIR=/mnt/sdcard/roms/ports/zombieattack
cd "$PORT_DIR"
export ZOMBIE_DATA="$PORT_DIR/data"
exec ./zombie-attack.mipsel "$@"
