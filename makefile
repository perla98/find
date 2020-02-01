C = gcc
CFLAGS = -Wall -c -g -ggdb
LDFLAGS= -Wall -g -ggdb

find : find.c find.h
	cc -c find.c
