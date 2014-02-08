#! /usr/bin/env

LD_PRELOAD="$(pwd)/extern/openssl/libcrypto.so" valgrind $@
