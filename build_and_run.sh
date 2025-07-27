#!/bin/sh

echo "[Cleaning]"
make clean
echo "[Building]"
make CFLAGS="-Wall -Wextra -O3"
if [ $? != 0 ]; then
  echo "[Build failed]"
  exit 1
fi
echo "[Running]"
./build/gbemu
