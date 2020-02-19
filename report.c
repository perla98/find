
#include "report.h"

int prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int fileEnd(const char *str, const char *suffix)
{
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void report(char* output, char* show, char* showfile){

    char* const wd = "WORD ";
	char* const fl = "FILE ";

	char* words = (char*)malloc(1024);

	FILE* f = fopen(output, "r");
	char* searchStr = (char*)malloc(sizeof(words));
	char* str = (char*)malloc(sizeof(words));
	strcat(searchStr, wd);
	strcat(searchStr, show);
	int prefixFlag, fileFlag = 0;
	while (fgets(words, 1024, f)) {

		strtok(words, "\n");
		if(prefix(wd, words) ) {
			if(strcmp(words, searchStr) == 0)
				prefixFlag = 1;
			else
				prefixFlag = 0;

		}

		if(strlen(showfile) > 0) {
			if(prefixFlag) {
				if(prefix(fl, words) ) {
					if(fileEnd(words, showfile)) 
						fileFlag = 1;
			 		else
						fileFlag = 0;
				}
			}
		} 
		else 
			fileFlag = 1;
			

		if(prefixFlag && fileFlag) {
			strcat(str, words);
			strcat(str, "\n");
		}
	}

	printf("%s", str);
}