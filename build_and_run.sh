#!/bin/sh

echo "[Cleaning]"
make clean
echo "[Building]"
make
if [ $? != 0 ]; then
  echo "[Build failed]"
  exit 1
fi
echo "[Running]"
./build/gbemu
