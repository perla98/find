// find.h: file di inclusione per file di inclusione di sistema standard,
// o file di inclusione specifici del progetto.

#pragma once

typedef struct word {
	char* name;
	int *totalOccurrences;
	struct fileOccurrences* fo;
	struct word *next;
} word;

typedef struct fileOccurrences {
	char* filePath;
	int x;
	int y;
	struct fileOccurrences* next;
} fileOccurrences;

typedef struct Input {
	char* path;
	int rec;
	struct Input* next;
} Input;
