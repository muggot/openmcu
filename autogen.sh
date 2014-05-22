#!/bin/sh

echo "Generating build scripts..."
aclocal
autoconf
automake --add-missing --copy

cd libs/libzrtp/ && ./bootstrap.sh

