#include "utils.h"

#include <stdlib.h>
#include <regex.h>
#include <string.h>

char* read_file(FILE* fd) {
	int length;
	char* text;

	/* Récupère tout le code dans un tableau */
	fseek(fd, 0, SEEK_END);
	length = ftell(fd);
	rewind(fd);

	if ((text = calloc(length, sizeof(char) + 1)) == NULL) {
			perror("Error calloc\n");
			exit(1);
	}

	size_t read_len = fread(text, 1, length, fd);
	text[read_len]='\0';

	fclose(fd);

	return text;
}

void freeTokens(TOKEN_LIST *tokenList) {
	for (int i = 0; i < tokenList->size; i++) {
		if (tokenList->tokens[i].ownstr) {
            free(tokenList->tokens[i].value.value_str);
        }
	}
	free(tokenList->tokens);
}

void freeRegexes(regexList* regexes) {
	regfree(&(regexes->integer));
	regfree(&(regexes->floating));
	regfree(&(regexes->comment));
	regfree(&(regexes->charac));
}

char* extractSubString(char** string, int len) {
	/* Allocation sur la heap pour pouvoir sauvegarder cette donnée (+1 pour le '\0') */
	char* subString = malloc((len + 1) * sizeof(char));
	if (subString == NULL)
	{
		perror("Error malloc\n");
		exit(1);
	}
	

	/* Copie de la string d'origine troncatée à la bonne longueur dans subString */
	strncpy(subString, *string, len);

	/* Ajout du '\0' */
	subString[len] = '\0';

	return subString;
}

void initRegexes(regexList* regexes) {
	if(regcomp(&(regexes->floating), "^([0-9]+\\.[0-9]+)", REG_EXTENDED)) {
		perror("Error regcomp float\n");
		exit(1);
	}

	if(regcomp(&(regexes->integer), "^[+-]?[0-9]+", REG_EXTENDED)) {
		perror("Error regcomp integer\n");
		exit(1);
	}

	if(regcomp(&(regexes->comment), "^\\/\\/.*", REG_EXTENDED)) {
		perror("Error regcomp comment\n");
		exit(1);
	}

	if(regcomp(&(regexes->charac), "^[[:alpha:]][a-zA-Z_0-9]*", REG_EXTENDED)) {
		perror("Error regcomp character\n");
		exit(1);
	}
}

TOKEN_LIST *initTokenList(char** text) {
	/* Allocation de 256 tokens puis realloc */
	TOKEN_LIST *tokenList = malloc(sizeof(TOKEN_LIST));

	if (tokenList == NULL)
	{
		printf("Error tokenList malloc.\n");
		exit(1);
	}
	
	
	tokenList->size = 256;
	tokenList->indexToken = 0;

	if ((tokenList->tokens = calloc(tokenList->size, sizeof(TOKEN))) == NULL) {
		perror("Error calloc\n");
		exit(1);
	}

	return tokenList;
}