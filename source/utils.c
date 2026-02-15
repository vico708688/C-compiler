#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

char* read_file(FILE* fd) {
	int length;
        char* text;

        /* Récupère tout le code dans un tableau */
        fseek(fd, 0, SEEK_END);
        length = ftell(fd);
        rewind(fd);

        if ((text = malloc(length * sizeof(char) + 1)) == NULL) {
                perror("Error malloc\n");
                exit(1);
        }

        size_t read_len = fread(text, 1, length, fd);
        text[read_len]='\0';

	fclose(fd);

	return text;
}

int lenText(char** ptext) {
	int counter = 0;
	char* text = *ptext;

	while (!isAtEnd(&text)) {
		if (*text == ' ' || *text == '\n') counter++;
		text++;
	}

	return counter;
}

void freeAll(TOKEN_LIST* tokenList, regex_t* regex) {
	freeTokens(tokenList);
	freeRegexes(regex);
}

void freeTokens(TOKEN_LIST *tokenList) {
	free(tokenList->tokens);
}

void freeRegexes(regex_t* regex) {
	regfree(regex);
}

char* extractSubString(char** string, int len) {
	/* Allocation sur la heap pour pouvoir sauvegarder cette donnée (+1 pour le '\0') */
	char* subString = malloc((len + 1) * sizeof(char));

	/* Copie de la string d'origine troncatée à la bonne longueur dans subString */
	strncpy(subString, *string, len);

	/* Ajout du '\0' */
	subString[len] = '\0';

	return subString;
}