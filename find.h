// find.h: file di inclusione per file di inclusione di sistema standard,
// o file di inclusione specifici del progetto.

#pragma once

typedef struct word {
	char* name;
	int total;
	char* filename;
	int** position;
};
