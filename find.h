#ifndef __FIND_H
#define __FIND_H
#include <stdio.h> 
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h> 
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "knuthMorrisPratt.h"
#define MAX_WORD 200
#define MAX_OCC 400


struct fileOccurrences {
	struct Input* filePath;
	int total;
	int x[MAX_OCC];
	int y[MAX_OCC];
};


struct word {
	char* name;
	struct fileOccurrences* fo;
};

typedef struct Input {
	char* path;
	int rec;
	struct Input* next;
} Input;


void find(char* wordFilePath,char* inputFilePath,char* outputFilePath, char* extention, int verboseMode);

#endif