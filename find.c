
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
	FILE* f = fopen(filename, "w");
	fprintf(f, "%s", string);
	fclose(f);
}

int countWordOccurrences(struct word w)
{
	int total = 0;
	for(int y=0; y<MAX_OCC; y++)
		if(w.fo[y].filePath == NULL) 
			break;
		else
			total += w.fo[y].total;
			
	return total;
}

void concatChar(char* target,const char* scr,const char* nextChar)
{
	strcat(target, scr);
	strcat(target, nextChar);
}	

void concatInt(char* target,const int scr,const char* nextChar)
{
	char* temp = (char*)malloc(sizeof(char)*sizeof(int)*2);
	sprintf(temp, "%d", scr);
	concatChar(target, temp, nextChar);
	free(temp);
}

void writeWords(struct word* listW, char* path)
{
	char* const DEFAULT_SEPARATOR = "\n";
	char* str = (char*)malloc(sizeof(char)*(sizeof(struct word)*MAX_WORD + sizeof(struct fileOccurrences)*MAX_OCC));

	for(int i=0; i<MAX_WORD; i++)
	{
		if(listW[i].name == 0) break;

		strcat(str, "WORD ");
		concatChar(str, listW[i].name, DEFAULT_SEPARATOR);
		strcat(str, "TOTAL ");
		concatInt(str, countWordOccurrences(listW[i]), DEFAULT_SEPARATOR);

		for(int y=0; y<MAX_OCC; y++)
		{
			if(listW[i].fo[y].filePath == NULL) break;
			strcat(str, "FILE ");
			concatChar(str, listW[i].fo[y].filePath->path, DEFAULT_SEPARATOR);
			strcat(str, "OCCURRENCES ");
			concatInt(str, listW[i].fo[y].total, DEFAULT_SEPARATOR);
			
			for(int z=0; z<MAX_OCC; z++)
			{
				if(listW[i].fo[y].x[z] == 0 && listW[i].fo[y].y[z] == 0) break;
				
				concatInt(str, listW[i].fo[y].x[z], " ");
				concatInt(str, listW[i].fo[y].y[z], DEFAULT_SEPARATOR);
			}
		}    
	}

	if(strlen(path) > 0)
		writeToFile(path, str);
	else
		printf("%s\n", str);

	free(str);
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
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

struct fileOccurrences* scanFile(struct Input* input, char* name, char* extention, int verboseMode)
{
	struct fileOccurrences* foc = (struct fileOccurrences*)malloc(MAX_OCC * sizeof(struct fileOccurrences));
	int fileCount = 0;
	while (input != NULL) {
				if(!EndsWith(input->path, extention))
					if (opendir(input->path) == NULL) {
						if(verboseMode)
							printf("Inizio elaborazione file: %s\n", input->path);

						foc[fileCount].total = countWord(readFile(input->path), name, foc[fileCount].x, foc[fileCount].y);
						foc[fileCount].filePath = input;
						
						if(verboseMode)
							printf("Fine elaborazione file: %s\n", input->path);

						fileCount++;
					}
				input = input->next;
			}
	return foc;
}	

void copyWord(struct word* listW, char* name, struct fileOccurrences* foc)
{
	struct word* w = (struct word*)malloc(sizeof(struct word));

	w->name = (char*)malloc(strlen(name));

	strcpy(w->name, name);

	w->fo = (struct fileOccurrences*)malloc(MAX_OCC * sizeof(struct fileOccurrences));

	memcpy(w->fo, foc, MAX_OCC * sizeof(struct fileOccurrences));

	addWordToList(listW, w);
}	

void find(char* wordFilePath,char* inputFilePath,char* outputFilePath, char* extention, int verboseMode)
{
	listFiles(inputFilePath);
	struct Input * input = &inputList;
	struct word listW[MAX_WORD];
	char* words = (char*)malloc(sizeof(readFile(wordFilePath))); 
	FILE* f = fopen(wordFilePath, "r");
	int processedWord = 0;

		while(fgets(words, sizeof(words), f))
		{
			strtok(words, "\n");
			
			if(verboseMode)
				printf("Inizio elaborazione parola: %s\n", words);

			copyWord(listW, words, scanFile(input, words, extention, verboseMode));
			
			if(verboseMode)
				printf("Fine elaborazione parola: %s\n", words);

			input = &inputList;

			processedWord++;
		}
	fclose(f);

	qsort(listW, processedWord, sizeof(struct word), CompareWord); 

	if(verboseMode)
		printf("Tutte le elaborazioni sono terminate.\n");
	
	writeWords(listW, outputFilePath);
}
