#!/bin/sh

if [ ! -x configure ]; then
   echo "Cannot find configure script!" >&2
   echo "You should run ./autogen.sh before!" >&2
   exit 1
fi

set -x

./configure $@ && \
make --silent --no-print-directory && \
make test --silent --no-print-directory

