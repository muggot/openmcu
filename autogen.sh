#!/bin/sh

#AM_VERSION="1.10"
if ! type aclocal-$AM_VERSION 1>/dev/null 2>&1; then
	AUTOMAKE=automake
	ACLOCAL=aclocal
else
	ACLOCAL=aclocal-${AM_VERSION}
	AUTOMAKE=automake-${AM_VERSION}
fi

echo "Generating build scripts..."
$ACLOCAL
$AUTOMAKE --add-missing --copy
autoconf

cd libs/libzrtp/ && ./bootstrap.sh

