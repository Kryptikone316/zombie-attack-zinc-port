#!/bin/sh
set -u

PORT_DIR=/mnt/sdcard/roms/ports/zombieattack
cd "$PORT_DIR"
rm -f /mnt/sdcard/cubegm/port_error.txt
export ZOMBIE_DATA="$PORT_DIR/data"

# The device has no RTC. Preserve a previous run and start a fresh diagnostic
# log so repeated hardware tests can be compared without timestamp assumptions.
[ -f log.txt ] && cp log.txt log.prev.txt
: > log.txt
rc=0
"$PORT_DIR/zombie-attack.mipsel" < /dev/null >> log.txt 2>&1 || rc=$?
echo "EXIT CODE: $rc" >> log.txt
exit "$rc"
