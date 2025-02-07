#!/bin/bash
gcc -v -g -gdwarf-4 -Og -O0 -L/lib -L/usr/lib -L /usr/lib/arm-linux-gnueabihf -I/usr/include -I/usr/include/SDL2  main.c -o main.out  -ldl -lSDL2 -lSDL2_image
