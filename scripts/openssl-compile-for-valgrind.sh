#! /usr/bin/env bash


# from OpenSSL FAQ: https://www.openssl.org/support/faq.html#PROG14
#
#   14. Why does Valgrind complain about the use of uninitialized data?
#
#   When OpenSSL's PRNG routines are called to generate random numbers the
#   supplied buffer contents are mixed into the entropy pool: so it technically
#   does not matter whether the buffer is initialized at this point or not.
#   Valgrind (and other test tools) will complain about this. When using
#   Valgrind, make sure the OpenSSL library has been compiled with the PURIFY
#   macro defined (-DPURIFY) to get rid of these warnings.

# so this script downloads the OpenSSL version, specified by the
# extern/openssl-*.sha1 file and compiles libcrypto with the PURIFY flag.

set -e

OPENSSL_SHA1_FILE=`ls extern/openssl-*.sha1`
OPENSSL_SHA1_BASENAME=$(basename "$OPENSSL_SHA1_FILE")

# strip of .sha1
OPENSSL_TAR_NAME="${OPENSSL_SHA1_BASENAME%.*}"
OPENSSL_TAR="/tmp/$OPENSSL_TAR_NAME"

# strip of tar.gz from openssl-1.0.1f.tar.gz
OPENSSL_SOURCES="$(pwd)/extern/${OPENSSL_TAR_NAME%.*.*}"
TMP_DIR="/tmp"

LIBRARY_PATH="$(pwd)/libs"
mkdir -pv $LIBRARY_PATH

TMP_FILE="$TMP_DIR/$OPENSSL_TAR_NAME"
if ( [ ! -e $OPENSSL_TAR ] && [ ! -d $OPENSSL_SOURCES ] ); then
    echo "> Downloading $OPENSSL_TAR_NAME\" from \"http://www.openssl.org/source/$OPENSSL_TAR_NAME\""
    curl -o "$TMP_FILE" "http://www.openssl.org/source/$OPENSSL_TAR_NAME"
    cp $OPENSSL_SHA1_FILE $TMP_DIR
    (cd $TMP_DIR; sha1sum --check "$TMP_DIR/$OPENSSL_SHA1_BASENAME")
    if [ "$?" != "0" ]; then
        echo "Could not verify sha1sums"
        exit 1
    fi
fi
if [ ! -d $OPENSSL_SOURCES ]; then
    mkdir -pv extern/
    tar -C extern/ -xf "$OPENSSL_TAR"
fi


ln -sf "${OPENSSL_SOURCES}/" extern/openssl
(cd $OPENSSL_SOURCES;
    ./Configure linux-x86_64 -g -DPURIFY shared

    make build_crypto
)
