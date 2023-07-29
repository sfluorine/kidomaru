#!/usr/bin/bash

clang -D_FILE_OFFSET_BITS=64 -Wall -O3 main.c lexer.c ast.c parser.c -o kidomaru
