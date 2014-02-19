#! /usr/bin/env bash

export LD_LIBRARY_PATH="$(pwd)/src/.libs"
LD_PRELOAD="$(pwd)/extern/openssl/libcrypto.so" valgrind $@
