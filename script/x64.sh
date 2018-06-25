#!/bin/bash
# the output file name
NAME=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)

# the clang executable to use for building
CLANG=w64-clang

# the destination output name
DST_NAME=hwid.exe

# Define build arguments
DEFINE_ARGS=""

# Linker build arguments
LINKER_ARGS="-Wl,--gc-sections -Wl,-allow-multiple-definition -Wl,--exclude-all-symbols"

# Compiler build arguments
COMPILER_ARGS="-s -std=c99 -lntoskrnl -lhal -fno-stack-check -fno-stack-protector -fno-exceptions -fno-rtti -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fno-unroll-loops -fmerge-all-constants"

# Source files to build to IR files
SRC_FILES="../main.c"

echo 'Building to to file' $NAME

# cleanup
rm build/*

# build
cd build
$CLANG $DEFINE_ARGS $SRC_FILES $COMPILER_ARGS -I/usr/x86_64-w64-mingw32/include/ddk $LINKER_ARGS -o $NAME

# rename result
mv ../build/$NAME ../build/$DST_NAME