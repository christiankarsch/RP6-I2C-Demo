#!/bin/sh

SUFFIX=or32
CC=or32-linux-gcc
CFLAGS="-Wall -ansi"
LDFLAGS="-Wall"


echo --- clear o-Files ---
rm ./o-$SUFFIX -r -f
echo

echo --- clear bin-Files ---
rm ./bin-$SUFFIX -r -f
echo

mkdir ./o-$SUFFIX/
mkdir ./o-$SUFFIX/lib/
mkdir ./o-$SUFFIX/lib/linux/
mkdir ./o-$SUFFIX/lib/linux/i2c/
mkdir ./o-$SUFFIX/lib/rp6/
mkdir ./bin-$SUFFIX/

echo --- Compile: lib/linux/i2c/linux_i2c.c ---
$CC $CFLAGS -c -o ./o-$SUFFIX/lib/linux/i2c/linux_i2c.o ./src/lib/linux/i2c/linux_i2c.c
echo

echo --- Compile: lib/rp6/rp6.c ---
$CC $CFLAGS -c -o ./o-$SUFFIX/lib/rp6/rp6.o ./src/lib/rp6/rp6.c
echo

echo --- Compile: rp6-i2c-demo.c ---
$CC $CFLAGS -c -o ./o-$SUFFIX/rp6-i2c-demo.o ./src/rp6-i2c-demo.c
echo

echo --- Linking: rp6-i2c-demo ---
$CC $LDFLAGS -o ./bin-$SUFFIX/rp6-i2c-demo \
 ./o-$SUFFIX/lib/linux/i2c/linux_i2c.o \
 ./o-$SUFFIX/lib/rp6/rp6.o \
 ./o-$SUFFIX/rp6-i2c-demo.o \

echo
