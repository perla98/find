
#include "knuthMorrisPratt.h"	


int countWord(char* s, char* word, int* X, int* Y) {
	
	char first = word[0];
	size_t i, j, count = 0;
	int f,row = 0;
	if (first == '\0')
		return 0;

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
				X[count-1] = row;
				Y[count-1] = i - f;
			}
		}
	}
	return count;
}