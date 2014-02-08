#! /usr/bin/env bash

LIBRARY_PATH="$(pwd)/libs"


if [ ! -f "$LIBRARY_PATH/libcrypto.so" ]; then
    ./scripts/openssl-compile-for-valgrind.sh
fi

./configure LDFLAGS="-L/$LIBRARY_PATH" \
    CFLAGS="-g -O0" CXXFLAGS="-g -O0"

make

cp -v src/.libs/libnotbit.* "$LIBRARY_PATH"
