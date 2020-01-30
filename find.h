﻿// find.h: file di inclusione per file di inclusione di sistema standard,
// o file di inclusione specifici del progetto.


typedef struct word {
	char* name;
	struct fileOccurrences* fo;
	struct word *next;
} word;

typedef struct fileOccurrences {
	struct Input* filePath;
	int x;
	int y;
	struct fileOccurrences* next;
} fileOccurrences;

typedef struct Input {
	char* path;
	int rec;
	struct Input* next;
} Input;
