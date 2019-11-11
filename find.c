// find.cpp: definisce il punto di ingresso dell'applicazione.
//

#include <stdio.h>
#include "find.h"

struct word generateNewWord(char* name, int total, char* filename, int** position)
{
	struct word w;

	w.name = name;
	w.total = total;
	w.filename = filename;
	w.position = position;

	return w;
}


char* readFile(char* filename)
{
	char* buffer = NULL;
	int string_size, read_size;
	FILE* handler = fopen(filename, "r");

	if (handler)
	{
		// Seek the last byte of the file
		fseek(handler, 0, SEEK_END);
		// Offset from the first to the last byte, or in other words, filesize
		string_size = (int)ftell(handler);
		// go back to the start of the file
		rewind(handler);

		// Allocate a string that can hold it all
		buffer = (char*)malloc(sizeof(char) * (string_size + 1));

		// Read it all in one operation
		read_size = fread(buffer, sizeof(char), string_size, handler);

		buffer[string_size] = '\0';

		if (string_size != read_size)
		{
			free(buffer);
			buffer = NULL;
		}

		fclose(handler);
	}

	return buffer;
}

void writeToFile(char* filename, char* string)
{
	FILE* f = fopen(filename, "a");
	fprintf(f, "%s", string);
	fclose(f);
}


size_t countWord(char* s, char* word) {
	char first = word[0];
	size_t i, j, count = 0;

	if (first == '\0')
		return strlen(s) + 1;

	for (i = 0; s[i] != '\0'; i++) 
		if (s[i] == first) {
			for (j = 1; word[j] != '\0' && s[i + j] == word[j]; j++)
				continue;
			if (word[j] == '\0')
				count++;
		}

	return count;
}


int main(int argc, char** argv)
{
	printf("Prova di scrittura \n");



	return 0;
}
