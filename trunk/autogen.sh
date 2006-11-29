#!/bin/sh

set -x

aclocal && \
autoheader -Wall && \
automake -Wall --foreign --add-missing && \
autoconf -Wall

