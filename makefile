CC = gcc
CFLAGS = -Wall -c -g -ggdb
LDFLAGS= -Wall -g -ggdb

main: main.o find.o knuthMorrisPratt.o report.o
	$(CC) $(LDFLAGS) -o find main.o find.o knuthMorrisPratt.o report.o
main.o : main.c find.h report.h
	$(CC) $(CFLAGS) -c main.c
find.o : find.c knuthMorrisPratt.h find.h
	$(CC) $(CFLAGS) find.c -o find.o
knuthMorrisPratt.o : knuthMorrisPratt.c knuthMorrisPratt.h report.h 
	$(CC) $(CFLAGS) knuthMorrisPratt.c -o knuthMorrisPratt.o
report.o: report.c report.h
	$(CC) $(CFLAGS) report.c -o report.o