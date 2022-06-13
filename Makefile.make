# Simple Makefile with: build, clean, and view targets
# Author: Michael Ibrahim
# Date  : June 2nd, 2022

build : 265inventory.c
   gcc 265inventory.c -o build

clean : build
   rm build

view : 265inventory.c
   less 265inventory.c