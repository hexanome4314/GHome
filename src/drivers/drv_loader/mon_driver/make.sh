#!/bin/sh

gcc -c -fPIC driver.c -o driver.o
gcc -shared -o libdriver.so.1.0.1 driver.o
