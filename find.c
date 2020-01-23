// find.c: definisce il punto di ingresso dell'applicazione.
//

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "find.h"
#include "dirent.h"

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

int countOccurrences(struct fileOccurrences* fo)
{
	int tot = 0;
	while (fo != NULL)
	{
		tot++;
		fo = fo->next;
	} 

	return tot;
}

void writeWords()
{
	char* DEFAULT_SEPARATOR = "\n";
	struct word * f = &wordList;
	while (f != NULL) {
		char* name = malloc(sizeof(char) * (sizeof(f->name) + sizeof(DEFAULT_SEPARATOR)));
		name = f->name;
		struct fileOccurrences* fo = f->fo;
		
		int tot = countOccurrences(fo);

		char* totOcc = (char*)malloc(sizeof(int) + sizeof(DEFAULT_SEPARATOR));
		sprintf(totOcc, "%d", tot);

		char* Occ = (char*)malloc((1024 + sizeof(DEFAULT_SEPARATOR) + sizeof(int) + sizeof(" ") + sizeof(int) + sizeof(DEFAULT_SEPARATOR)) * tot);

		 fo = f->fo;
		 char* str = (char*)malloc(sizeof(int));
		 Occ[0] = 0;
		while (fo != NULL)
		{
			//char* currentOcc = fo->filePath;
			strcat(Occ, fo->filePath->path);
			strcat(Occ, DEFAULT_SEPARATOR);
			sprintf(str, "%d", fo->x);
			strcat(Occ, str);
			strcat(Occ, " ");
			sprintf(str, "%d", fo->y);
			strcat(Occ, str);
			strcat(Occ, DEFAULT_SEPARATOR);

			fo = fo->next;
		}

		char* myTxt = (char*)malloc(sizeof(name) + sizeof(totOcc) + sizeof(Occ));

		myTxt[0] = 0;
		strcat(myTxt, name);
		strcat(myTxt, DEFAULT_SEPARATOR);
		strcat(myTxt, totOcc);
		strcat(myTxt, DEFAULT_SEPARATOR);
		strcat(myTxt, Occ);
		strcat(myTxt, DEFAULT_SEPARATOR);


		writeToFile("find.txt", myTxt);
		f = f->next;
	}
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
		temp = strtok(temp, "\n");
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

int CompareWord(const word* p, const word* q)
{
	return strcmp(p->name, q->name);
}

void addWordToList(struct word * w)
{
	if (wordList.name != NULL) {
		struct word* temp = &wordList;
		int pos = 0;
		while (temp != NULL && CompareWord(w, temp) > 0) {
			temp = temp->next;
			pos++;
		}

		temp = &wordList;
		
		for (int i = 0; i < pos - 1; i++)
			temp = temp->next;
			
		if (CompareWord(w, temp) < 0) {
			struct word cw = *temp;
			w->next = &cw;
			struct word x = *w;
			wordList = x;
		}
		else
			temp -> next = w;
	}
	else {
		w->next = NULL;
		wordList = *w;
	}
}

void insertPos(size_t* x, size_t* j, struct fileOccurrences* fo)
{
	fo->x = *x;
	fo->y = *j;
}

fileOccurrences countWord(char* s, char* word, struct fileOccurrences* fo) {
	char first = word[0];
	size_t i, j/*, count*/ = 0;
	int buffer_size = sizeof(s);

	if (first == '\0')
		//return strlen(s) + 1; 
		return;

	for (i = 0; s[i] != '\0'; i++) 
		if (s[i] == first) {
			for (j = 1; word[j] != '\0' && s[i + j] == word[j]; j++)
				continue;
			if (word[j] == '\0')
			{
				if (fo->x == NULL) {
					insertPos(i, j, fo);
				}
				else
				{
					struct fileOccurrences *temp = fo;
					struct Input* in = temp->filePath;
					while (temp->next != NULL) {
						temp = temp->next;
					}
					insertPos(i, j, temp);
					temp->filePath = in;
				}
			}
		}

	return *fo;
}

void find(char* wordFilePath,char* inputFilePath,char* outputFilePath)
{
	listFiles(inputFilePath);

}

int CheckInput(char* argv, char* charArg)
{
	return (argv)[3] != &charArg;
}

int main(int argc, char** argv)
{
	struct option long_options[] = {
	{"word", required_argument, 0, 'w'},
	{"path", required_argument, 0, 'p'},
    {"exclude", required_argument, 0, 'e'},
    {"history", required_argument, 0, 'h'},
    {"user", required_argument, 0, 'u'},
    {"group", required_argument, 0, 'g'},
    {"length", required_argument, 0, 'l'},
    {"noscan", no_argument, 0, 'n'},
    {"help", no_argument, 0, 'x'},
	{NULL, 0, NULL, 0}

	};

	char* word;
    char* path;
    char* extension = NULL;
    int opt;
    int option_index = 0;
    const char * short_options = "w:p:e:h:u:g:l:";

	while ((opt = getopt_long(argc, argv, short_options , long_options, &option_index)) != -1) {
        switch (opt) {
            case 'w': word = optarg; 
                break;
            case 'p': path = optarg; 
                break;
            case 'e':extension = optarg;
                break;
            case 'h':printf("Hai scelto H\n");
                break;
            case 'u':printf("Hai scelto U\n");
                break;
            case 'a': printf("Hai scelto G\n"); 
                break;
        }
    }

	//da rifare

	//listFiles("prova.txt");
	struct fileOccurrences fo1;
	struct fileOccurrences fo2;
	struct fileOccurrences fo3;
	word a;
	a.name = "ciao";

	Input n;
	n.path = "dfsdfsd";
	n.next = NULL;
	fo1.filePath = &n;
	fo1.x = 10;
	fo1.y = 15;
	fo1.next = NULL;
	a.fo = &fo1;
	word b;
	b.name = "xy";
	fo2.filePath = &n;
	fo2.x = 32;
	fo2.y = 15;
	fo2.next = NULL;
	b.fo = &fo2;
	word c;
	c.name = "al";
	fo3.filePath = &n;
	fo3.x = 32;
	fo3.y = 45;
	fo3.next = NULL;
	c.fo = &fo3;

	addWordToList(&a);
	addWordToList(&b);
	addWordToList(&c);
	
	writeWords();

	return 0;
}
