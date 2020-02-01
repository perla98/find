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

void writeToFile(const char* filename, char* string)
{
	FILE* f = fopen(filename, "w");
	fprintf(f, "%s", string);
	fclose(f);
}

int countOccurrences(struct fileOccurrences* fo)
{
	int tot = 0;
	//struct fileOccurences *temp = &fo;
	while (fo->next != NULL)
	{
		tot++;
		fo = fo->next;
	} 
	
	return tot;
}

void writeWords(char* path)
{
	char* DEFAULT_SEPARATOR = "\n";
	struct word * f = &wordList;
	const char* wd = "WORD ";
	const char* to = "TOTAL "; 
	const char* fl = "FILE ";
	/*char* name;
	char* totOcc;
	char* Occ ;
	char* str ;*/
	while (f != NULL) {
		char* name = malloc(sizeof(char) * (sizeof(wd) + sizeof(f->name) + sizeof(DEFAULT_SEPARATOR)));
		name = f->name;
		struct fileOccurrences* fo = f->fo;
		
		int tot = countOccurrences(fo);
		
		char* totOcc = malloc(sizeof(char)*(sizeof(to) + sizeof(int) + sizeof(DEFAULT_SEPARATOR)));
		sprintf(totOcc, "%d", tot);

	    char* Occ = (char*)malloc((sizeof(fl) + 1024 + sizeof(DEFAULT_SEPARATOR) + sizeof(int) + sizeof(" ") + sizeof(int) + sizeof(DEFAULT_SEPARATOR)) * tot);
		
		fo = f->fo;
		char* str = (char*)malloc(sizeof(int));
		Occ[0] = 0;
		
		while (fo->next != NULL)
		{
			//char* currentOcc = fo->filePath;
			strcat(Occ, fl);
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
		
		
		free(name);
		free(totOcc);
		free(Occ);
		free(str);
		//free(myTxt);
		
		

		f = f->next;
	}
}

int checkDir(struct dirent * d)
{
	return d->d_type == DT_DIR;
}

void addFileOccurrences(struct fileOccurrences fo, int x, int y)
{
	struct fileOccurrences* temp = &fo;

	if (fo.x != 0 && fo.y != 0) {

		struct fileOccurrences newFo;

		while (temp->next != NULL)
			temp = temp->next;
		
		newFo.x = x;
		newFo.y = y;
		newFo.filePath = fo.filePath;
		newFo.next = NULL;

		temp->next = &newFo;
	}
	else {
		fo.x = x;
		fo.y = y;
		fo.next = NULL;
	}
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

void insertPos(int* x, int* j, struct fileOccurrences* fo)
{
	fo->x = *x;
	fo->y = *j;
}

int countWord(char* s, char* word, struct fileOccurrences fo) {
	char first = word[0];
	size_t i, j, count = 0;
	int row = 0;
	if (first == '\0')
		return strlen(s) + 1;

	for (i = 0; s[i] != '\0'; i++) 
		if(s[i] == '\n')
		row++;

		if (s[i] == first) {
			for (j = 1; word[j] != '\0' && s[i + j] == word[j]; j++) 
				continue;
			if (word[j] == '\0') 
			{
				count++;
				addFileOccurrences(fo, row, i + j);
			}
		}

	return count;
}

void find(char* wordFilePath,char* inputFilePath,char* outputFilePath, char* extention)
{
	listFiles(inputFilePath);
	struct Input * input = &inputList;
	
	char* words = malloc(sizeof(readFile(wordFilePath))); 
	FILE* f = fopen(wordFilePath, "r");
	int i= 0;
	while (input != NULL) {
		//input->path;
		//printf("%d", ++i);

		if(verboseMode)
				printf("Inizio elaborazione file: %s\n", input->path);
		char* read = readFile(input->path); 
		
		
		while(fgets(words, sizeof(words), f))
		{

			struct fileOccurrences foc;
			struct word w;

			if(verboseMode)
				printf("Inizio elaborazione parola: %s\n", words);
			//elaborazione non funzionante
			foc.filePath = input;
			foc.x = 0;
			foc.y = 0;
			countWord(read, words, foc);
			w.fo = &foc;
			w.name = words;
			//printf("%d\n", w.fo->x);
			//printf("%d\n", w.fo->y);
			addWordToList(&w);
			//fine elaborazione
			if(verboseMode)
				printf("Fine elaborazione parola: %s\n", words);
		}

		if(verboseMode)
				printf("Fine elaborazione file: %s\n", input->path);
		
		//printf("new input");
		input = input->next;
		
	}

	if(verboseMode)
		printf("Tutte le elaborazioni sono terminate.\n");

	writeWords(outputFilePath);
	
}

void report(char* output, char* show, char* showfile){

}

int main(int argc, char** argv)
{
	//printf("avvio");
	
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
    char* extension;
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
		/*printf(word);
		printf(path);
		printf(output);*/
		find(word, path, output, extension);
	}
	else if(show != 0 && showFile != 0){
		/*printf(show);
		printf(showFile);
		printf(output);*/
		report(output, show, showFile);
	}
	else 
		printf("wrong args\n");
	
	return 0;
}
