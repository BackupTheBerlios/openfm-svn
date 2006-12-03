#!/bin/sh

set -x

aclocal && \
autoheader -Wall && \
automake -Wall -Wno-portability --foreign --add-missing && \
autoconf -Wall

