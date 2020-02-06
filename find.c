// find.c: definisce il punto di ingresso dell'applicazione.
//

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include "find.h"


#ifdef WIN32
	char* pathFormat = "%s\\%s";
#else
	char* pathFormat = "%s/%s";
#endif

int verboseMode = 0;

int outputMode = 0;

const char* wd = "WORD ";
const char* to = "TOTAL "; 
const char* fl = "FILE ";

word* listW;

Input inputList;

//word* wordList;

char* readFile(char* filename)
{
	char* buffer = NULL;
	int string_size, read_size;
	FILE* handler = fopen(filename, "r");
	
	if (handler)
	{
		fseek(handler, 0, SEEK_END);
		
		string_size = (int)ftell(handler);

		rewind(handler);

		buffer = (char*)malloc(sizeof(char) * (string_size + 1));

		read_size = fread(buffer, sizeof(char), string_size, handler);

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

void writeWords(char* path)
{
	char* DEFAULT_SEPARATOR = "\n";
	char* name;
	char* totOcc;
	char* Occ ;
	char* str ;
	for(int i=0; i< MAX_WORD; i++){
		
		struct word * f = &listW[i];
		if(f->name == 0)
			break;
		name = malloc(sizeof(char) * (sizeof(wd) + sizeof(f->name) + sizeof(DEFAULT_SEPARATOR)));
		name = f->name;
		struct fileOccurrences * fo = f->fo;
		
		int tot = 0;

		for (int x =0; x < MAX_OCC; x++)
		{
			if(fo[x]. x == 0 && fo[x].y == 0)
				break;			
			
			tot++;
		}
		
		totOcc = malloc(sizeof(char)*(sizeof(to) + sizeof(int) + sizeof(DEFAULT_SEPARATOR)));
		sprintf(totOcc, "%d", tot);

	    Occ = (char*)malloc((sizeof(fl) + 1024 + sizeof(DEFAULT_SEPARATOR) + sizeof(int) + sizeof(" ") + sizeof(int) + sizeof(DEFAULT_SEPARATOR)) * tot);
		
		char* str = (char*)malloc(sizeof(int));
		Occ[0] = 0;
		
		for (int x =0; x < MAX_OCC; x++)
		{
			if(fo[x]. x == 0 && fo[x].y == 0)
				break;
			
			strcat(Occ, fl);
			strcat(Occ, fo[x].filePath->path);
			strcat(Occ, DEFAULT_SEPARATOR);
			sprintf(str, "%d", fo[x].x);
			strcat(Occ, str);
			strcat(Occ, " ");
			sprintf(str, "%d", fo[x].y);
			strcat(Occ, str);
			strcat(Occ, DEFAULT_SEPARATOR);

			
		}
		
		char*  myTxt = (char*)malloc(sizeof(name) + sizeof(totOcc) + sizeof(Occ));

		myTxt[0] = 0;
		strcat(myTxt, wd);
		strcat(myTxt, name);
		strcat(myTxt, DEFAULT_SEPARATOR);
		strcat(myTxt, to);
		strcat(myTxt, totOcc);
		strcat(myTxt, DEFAULT_SEPARATOR);
		strcat(myTxt, Occ);
		strcat(myTxt, DEFAULT_SEPARATOR);
		

		if(outputMode)
			writeToFile(path, myTxt);
		else
			printf("%s", myTxt);
		
		free(Occ);
		free(name);
		free(totOcc);
		
		free(str);
	}
}

int checkDir(struct dirent * d)
{
	return d->d_type == DT_DIR;
}

void addFileOccurrences(struct fileOccurrences *fo, int x, int y, Input* in)
{
	for(int i=0; i <MAX_OCC; i++)
		if(fo[i].x == 0 && fo[i].y == 0) {
				fo[i].x = x;
				fo[i].y = y;
				fo[i].filePath = in;
				break;
			}	
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
	Input temp = inputList;

	while (1)
	{
		Input dir;

		if (temp.rec == 1)
		{
			DIR* d = opendir(temp.path);
			struct dirent* entry;
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

int CompareWord(const void* p, const void* q)
{
	struct word* a = (word*)p;
	struct word* b = (word*)q;
	return strcmp(a->name, b->name);
}

void addWordToList(struct word* w)
{
	for(int i = 0; i < MAX_WORD; i++)
		if(listW[i].name == 0) {
			listW[i] = *w;
				break;
			}
}

void insertPos(int* x, int* j, struct fileOccurrences* fo)
{
	fo->x = *x;
	fo->y = *j;
}

fileOccurrences* countWord(Input* in, char* word, struct fileOccurrences* fo) {
	
	fo->x = 0;
	fo->y = 0;
	char* s = readFile(in->path); 
	char first = word[0];
	size_t i, j, count = 0;
	int f,row = 0;
	if (first == '\0')
		return fo;

	for (i = 0; s[i] != '\0'; i++) {
		if(s[i] == '\n'){
			row++;
			f = i;
		}
		
		if (s[i] == first) {
			for (j = 1; word[j] != '\0' && s[i + j] == word[j]; j++) 
				continue;
			if (word[j] == '\0') 
			{
				count++;
				addFileOccurrences(fo, row, i- f, in);
			}
		}
	}
	return fo;
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

void find(char* wordFilePath,char* inputFilePath,char* outputFilePath, char* extention)
{
	listW = malloc(sizeof(struct word)* MAX_WORD);
	listFiles(inputFilePath);
	struct Input * input = &inputList;
	
	char* words = (char*)malloc(sizeof(readFile(wordFilePath))); 
	FILE* f = fopen(wordFilePath, "r");
	int i = 0;
	struct word* w;
	struct fileOccurrences* foc;

#ifdef DEBUG
	foc = malloc(sizeof(struct  fileOccurrences) * MAX_OCC);
	input->path = "test.txt" //contiene due volte la parola ciao
	foc = countWord(input, "ciao", foc);
	int tot = 0;
	if(fo[x]. x == 0 && fo[x].y == 0) {
				break;			
			
			tot++;	
	}

	printf("Il risultato deve essere 2: %d\n", tot);
#endif

		while(fgets(words, sizeof(words), f))
		{
			w = malloc(sizeof(struct  word));
			foc = malloc(sizeof(struct  fileOccurrences) * MAX_OCC);
			strtok(words, "\n");
			if(verboseMode)
				printf("Inizio elaborazione parola: %s\n", words);


			while (input != NULL) {
				if(!EndsWith(input->path, extention)) {

				DIR* d = opendir(input->path);
	
				if (d == NULL) {
						if(verboseMode)
							printf("Inizio elaborazion_e file: %s\n", input->path);

						foc = countWord(input, words, foc);
						
						if(verboseMode)
						printf("Fine elaborazione file: %s\n", input->path);

				
					}
				}
				input = input->next;
			}

			w->fo = malloc(sizeof(struct  fileOccurrences) *MAX_OCC);

			for(int i =0; i <MAX_OCC; i++)
				w->fo[i] = foc[i];

			w->name = (char*)malloc(sizeof(sizeof(words)));
			strcpy(w->name, words);

			addWordToList(w);
			
			free(w);
			if(verboseMode)
				printf("Fine elaborazione parola: %s\n", words);

			input = &inputList;

			free(foc);
			i++;
		}
	fclose(f);

	qsort(listW, i, sizeof(struct word), CompareWord); 

	if(verboseMode)
		printf("Tutte le elaborazioni sono terminate.\n");
	
	writeWords(outputFilePath);
	
}

int prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void report(char* output, char* show, char* showfile){
	char* words = (char*)malloc(1024);

	FILE* f = fopen(output, "r");
	char* searchStr = (char*)malloc(sizeof(words));
	char* str = (char*)malloc(sizeof(words));
	strcat(searchStr, wd);
	strcat(searchStr, show);
	int prefixFlag = 0;
	while (fgets(words, 1024, f)) {


		strtok(words, "\n");
		if(prefix(wd, words) )
			if(strcmp(words, searchStr) == 0)
				prefixFlag = 1;
			else
				prefixFlag = 0;

		if(prefixFlag) {
			strcat(str, words);
			strcat(str, "\n");

		}
	}
	//fclose(f);
	if(showfile != 0)
		writeToFile(showfile, str);
	else
		printf(str);
}

int main(int argc, char** argv)
{	
	struct option long_options[] = {
	{"word", required_argument, 0, 'w'},
	{"input", required_argument, 0, 'i'},
	{"output", required_argument, 0, 'o'},
    {"exclude", required_argument, 0, 'e'},
    {"verbose", no_argument, 0, 'v'},
    {"report", required_argument, 0, 'r'},
    {"show", required_argument, 0, 's'},
    {"file", required_argument, 0, 'f'},
	{NULL, 0, NULL, 0}

	};

	char* word;
    char* path;
	char* output;
    char* extension = "...";
	char* show;
	char* showFile;
    int opt;
    int option_index = 0;
    const char * short_options = "w:i:o:e:v:r:s:f:x";
	
	while ((opt = getopt_long(argc, argv, short_options , long_options, &option_index)) != -1) {
        switch (opt) {
            case 'w': word = optarg; 
                break;
            case 'i': path = optarg; 
                break;
			case 'o': 	outputMode = 1;
						output = optarg;
				break;
			case 'e': extension = optarg;
                break;
			case 'v': verboseMode = 1;
                break;	
            case 'r': output = optarg;
                break;
            case 's': show = optarg;
                break;
            case 'f': showFile = optarg; 
                break;
        }
    }
	
	if(word !=NULL && word != 0 && path != 0 && path !=NULL){
		find(word, path, output, extension);
	}
	else if(output != 0 && show != 0){
		report(output, show, showFile);
	}
	else 
		printf("wrong args\n");
	
	return 0;
}
