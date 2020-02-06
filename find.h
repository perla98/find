
const int MAX_WORD = 200;

const int MAX_OCC = 400;


typedef struct fileOccurrences {
	struct Input* filePath;
	int x;
	int y;
} fileOccurrences;


typedef struct word {
	char* name;
	struct fileOccurrences* fo;
} word;

typedef struct Input {
	char* path;
	int rec;
	struct Input* next;
} Input;


void find(char* wordFilePath,char* inputFilePath,char* outputFilePath, char* extention);

void report(char* output, char* show, char* showfile);