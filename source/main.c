#include "compiler.h"

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage : %s <file_name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE* file = fopen(argv[1], "r");
	if (file == NULL)
	{
		printf("Error opening file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	char* text = read_file(file);
	
	compile(text);

	return 0;
}