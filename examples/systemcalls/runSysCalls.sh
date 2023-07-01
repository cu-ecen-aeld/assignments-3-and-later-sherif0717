#!/bin/sh

rm -rf systemcalls testfile.txt
gcc systemcalls.c -o systemcalls
./systemcalls
