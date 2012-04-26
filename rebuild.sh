#! /bin/sh

make clean
make distclean

./autogen.sh && ./configure --enable-debug=yes --prefix=/usr && make



