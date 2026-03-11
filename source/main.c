#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "lexer.h"
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
	char* backup_text = text; /* Modification de pointeur text dans lexer(), il faut donc sauvegarder le pointeur d'origine */

	TOKEN_LIST tokenList = initTokenList(&text);

	/* Analyse du code */
	#ifdef DEBUG
    printf("Analyse lexicale :\n-------------------------------------------------------------------------------\n");
	#endif
	lexer(&text, &tokenList);
	#ifdef DEBUG
    printf("Analyse syntaxique :\n-----------------------------------------------------------------------------\n");
	#endif
	parser(&tokenList);

	freeTokens(&tokenList);
	free(backup_text);

	return 0;
}