
#include "report.h"

int prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void report(char* output, char* show, char* showfile){

    const char* wd = "WORD ";

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
		printf(str);//writeToFile(showfile, str);
	else
		printf(str);
}