#!/bin/sh

echo "[Cleaning]"
make clean
echo "[Building]"
make CC=emcc \
  CFLAGS="-Wall -Wextra -O3" \
  LINK_FLAGS="-lSDL2 --embed-file ./testroms/helloworld.gb@helloworld.gb -s SINGLE_FILE -s ASYNCIFY" \
  TARGET="build/index.html"
if [ $? != 0 ]; then
  echo "[Build failed]"
  exit 1
fi
echo "[Starting web server]"
python3 -m http.server -d build
