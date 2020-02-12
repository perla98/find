#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <getopt.h>
#include "report.h"
#include "find.h"


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

    int verboseMode = 0;
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
			case 'o': output = optarg;
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
		find(word, path, output, extension, verboseMode);
	}
	else if(output != 0 && show != 0){
		report(output, show, showFile);
	}
	else 
		printf("wrong args\n");
	
	return 0;
}