
#include "find.h"

Input inputList;

char* readFile(char* filename)
{
	char* buffer = NULL;
	int string_size;
	FILE* handler = fopen(filename, "r");
	
	if (handler)
	{
		fseek(handler, 0, SEEK_END);
		
		string_size = (int)ftell(handler);

		rewind(handler);

		buffer = (char*)malloc(sizeof(char) * (string_size + 1));

		fread(buffer, sizeof(char), string_size, handler);

		buffer[string_size] = '\0';

		fclose(handler);
	}

	return buffer;
}

void writeToFile(const char* filename, char* string)
{
	FILE* f = fopen(filename, "a");
	fprintf(f, "%s", string);
	fclose(f);
}

void writeWords(struct word* listW, char* path)
{

	char* const wd = "WORD ";
	char* const to = "TOTAL "; 
	char* const fl = "FILE ";
	char* const oc = "OCCURRENCES ";
	char* const DEFAULT_SEPARATOR = "\n";

	printf(wd);
	printf(to);
	printf(fl);
	printf(oc);
	printf(DEFAULT_SEPARATOR);
	/*
	char* name;
	char* occ ;
	char* str ;
	
	for(int i=0; i<MAX_WORD; i++)
	{
		name=(char*)malloc(sizeof(wd)+sizeof(listW[i].name));
		name = wd;
		strcat(name, listW[i].name);

		for(int y=0; y<MAX_OCC; y++)
		{
		



		}
	}*/
}

void addInputToList(struct Input* dir)
{
	dir->next = NULL;

	if (inputList.path != NULL) {
		Input* temp = &inputList;

		while (temp->next != NULL)
			temp = temp->next;
		temp->next = malloc(sizeof(struct Input));
		temp->next = dir;
	}
	else {
		inputList = *dir;
	}
}

void fillRecursiveDirectory()
{
	char* const pathFormat = "%s/%s";

	Input temp = inputList;

	while (1)
	{
		Input dir;

		if (temp.rec == 1)
		{
			DIR* d = opendir(temp.path);
			struct dirent* entry;
			while ((entry = readdir(d)) != NULL) {
				if (entry !=NULL) {
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

	struct Input* dir = malloc(sizeof(struct Input));

	while(fgets(temp, 1024, f) != NULL) {
		temp = strtok(temp, "\n");
		char last = temp[strlen(temp) - 1];
		if (last == 'r') {
			dir->rec = 1;
			temp[strlen(temp) - 2] = '\0';
		}
		else 
			dir->rec = 0 ;
		dir->path = (char*)malloc(1024);
		strcpy(dir->path, temp);
		addInputToList(dir);
	}

	fillRecursiveDirectory();
}

void addWordToList(struct word* listW, struct word* w)
{
	for(int i = 0; i < MAX_WORD; i++)
		if(listW[i].name == 0) {
			listW[i] = *w;
				break;
			}
}

int CompareWord(const void* p, const void* q)
{
	struct word* a = (struct word*)p;
	struct word* b = (struct word*)q;
	return strcmp(a->name, b->name);
}

int EndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str); //abbiamo visto questo consiglio su internet di mettere size_t
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void find(char* wordFilePath,char* inputFilePath,char* outputFilePath, char* extention, int verboseMode)
{
	struct word listW[MAX_WORD];
	
	listFiles(inputFilePath);
	struct Input * input = &inputList;
	
	char* words = (char*)malloc(sizeof(readFile(wordFilePath))); 
	FILE* f = fopen(wordFilePath, "r");
	int i = 0;
	int fileCount;
	struct word* w;
	struct fileOccurrences* foc;

		while(fgets(words, sizeof(words), f))
		{
			fileCount = 0;
			w = malloc(sizeof(struct  word));
			foc = malloc(sizeof(struct fileOccurrences));
			strtok(words, "\n");
			if(verboseMode)
				printf("Inizio elaborazione parola: %s\n", words);


			while (input != NULL) {
				if(!EndsWith(input->path, extention)) {

				DIR* d = opendir(input->path);
	
				if (d == NULL) {
						if(verboseMode)
							printf("Inizio elaborazione file: %s\n", input->path);

						foc[fileCount].total = countWord(readFile(input->path), words, foc[fileCount].x, foc[fileCount].y);
						foc[fileCount].filePath = input;
						
						if(verboseMode)
							printf("Fine elaborazione file: %s\n", input->path);

				
					}
				}
				input = input->next;
			}

			w->name = words;

			w->fo = malloc(sizeof(struct fileOccurrences));

			memcpy(w->fo, foc, sizeof(struct fileOccurrences));

			addWordToList(listW, w);
			
			free(w);

			free(foc);

			if(verboseMode)
				printf("Fine elaborazione parola: %s\n", words);

			input = &inputList;

			i++;
		}
	fclose(f);

	qsort(listW, i, sizeof(struct word), CompareWord); 

	if(verboseMode)
		printf("Tutte le elaborazioni sono terminate.\n");
	
	writeWords(listW, outputFilePath);
	
}
