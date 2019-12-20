// find.c: definisce il punto di ingresso dell'applicazione.
//

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "dirent.h"
#include "find.h"

#ifdef WIN32
	char* pathFormat = "%s\\%s";
#else
	char* pathFormat = "%s/%s";
#endif

Input inputList;

word wordList;

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

		/*if (string_size != read_size)
		{
			free(buffer);
			buffer = NULL;
		}*/

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

int CompareWord(const word* p, const word* q)
{
	return strcmp(p->name, q->name);
}

int checkDir(dirent * d)
{
	return d->d_type == DT_DIR;
}

void addInputToList(struct Input* dir)
{
	dir->next = NULL;

	if (inputList.path != NULL) {
		Input* temp = &inputList;

		while (temp->next != NULL)
			temp = temp->next;

		temp->next = dir;
	}
	else {
		inputList = *dir;
	}
}

void fillRecursiveDirectory()
{
	Input temp = inputList;

	while (1)
	{
		Input dir;

		if (temp.rec == 1)
		{
			DIR* d = opendir(temp.path);
			dirent* entry;
			while ((entry = readdir(d)) != NULL) {
				if (checkDir(entry)) {
					char path[1024];
					if (strcmp(entry->d_name, ".") == 0  || 
						strcmp(entry->d_name, "..") == 0)
						continue;
					snprintf(path, sizeof(path), pathFormat, temp.path, entry->d_name);
					dir.path = path;
					dir.rec = 0;
					addInputToList(&dir);
				}
			}
		}
		if(temp.next != NULL)
			temp = *temp.next;
		else
			break;
	} 

}

void listFiles(char* filePath)
{
	char* temp = malloc(sizeof(char) * 1024);
	temp[0] = 0;
	FILE* f = fopen(filePath, "r");

	Input dir;

	while(fgets(temp, 1024, f) != NULL) {
		strtok(temp, "\n");
		char last = temp[strlen(temp) - 1];
		if (last == 'r') {
			dir.rec = 1;
			temp[strlen(temp) - 2] = '\0';
		}
		else 
			dir.rec = 0 ;
		dir.path = temp;
		addInputToList(&dir);
	}

	fillRecursiveDirectory();
}

void addWordToList(struct word* w)
{
	if (wordList.name != NULL) {
		struct word* temp = &wordList;
		int pos = 0;
		while (CompareWord(w, temp) || temp->next != NULL) {
			temp = temp->next;
			pos++;
		}
		struct word* x = &wordList;
		for (int i = 0; i < pos; i++)
			x = x->next;

		w->next = temp;

		x->next = w;
	}
	else {
		w->next = NULL;
		wordList = *w;
	}
}

size_t countWord(char* s, char* word, int** position) {
	char first = word[0];
	size_t i, j, count = 0;
	int buffer_size = sizeof(s);

	position = (int**)malloc(buffer_size * sizeof(int*));
	for (int i = 0; i < buffer_size; i++) position[i] = (int*)malloc(2 * sizeof(int));

	if (first == '\0')
		return strlen(s) + 1; 

	for (i = 0; s[i] != '\0'; i++) 
		if (s[i] == first) {
			for (j = 1; word[j] != '\0' && s[i + j] == word[j]; j++)
				continue;
			if (word[j] == '\0')
			{
				count++;
				position[count -1][0] = i; //add
				position[count -1][1] = j; //add
			}
		}

	return count;
}

int CheckInput(char* argv, char* charArg)
{
	return (argv)[3] != &charArg;
}

int main(int argc, char** argv)
{
	char charArg = '-';

	char* wordFilePath;
	char* inputFilePath;
	char* outputFilePath;

	while (*++argv)
		if ((*argv)[0] == charArg)
			switch ((*argv)[1]) 
			{
			default:
				printf("Unknown option -%c\n\n", (*argv)[1]);
				break;
			case 'w':
				if (CheckInput(argv, charArg))
					wordFilePath = argv[1];
				break;
			case 'i':
				if (CheckInput(argv, charArg))
					inputFilePath = argv[1];
				break;
			case 'o':
				if (CheckInput(argv, charArg))
					outputFilePath = argv[1];
				break;
			case 'e':
				break;
			case 'r':
				break;
				
			}

	//da rifare

	//listFiles("prova.txt");
	
	return 0;
}
