#!/bin/sh

set -o verbose

curr_platf=`uname -o`

if [ "$curr_platf" == "Cygwin" ]; then
    gcc -c -mno-cygwin -mwindows ../src/q6zip_uncompress.c -I../inc -o q6zip_uncompress.o && \
    gcc -o q6zip_ro_decompressor -mno-cygwin -mwindows q6zip_file_interface.c q6zip_uncompress.o -I../inc
elif [ "$curr_platf" == "GNU/Linux" ]; then
    gcc -c ../src/q6zip_uncompress.c  -I../inc/ -o q6zip_uncompress.o && \
    gcc -o q6zip_ro_decompressor q6zip_file_interface.c q6zip_uncompress.o -I../inc
fi
