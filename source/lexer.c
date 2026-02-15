#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

static int column = 0;
static int line = 0;

/* Ajoute un caractère au token voulu */
void addToken(TOKEN_LIST *tokenList, enum TOKEN_TYPE type, union TOKEN_VALUE value) {
	TOKEN token = { .type = type, .value = value, .line = line, .column = column };
	
	if (tokenList->indexToken >= tokenList->size) {
		TOKEN *tmp = NULL; /* temporary buffer for realloc */
		tokenList->size *= 2;
		tmp = realloc(tokenList->tokens, tokenList->size * sizeof(TOKEN));
		if (tmp == NULL) {
				freeTokens(tokenList);
				perror("Error realloc\n");
				exit(1);
		}
		tokenList->tokens = tmp;
	}
	tokenList->tokens[tokenList->indexToken] = token; /* tester s'il reste de la place dans la liste */
	(tokenList->indexToken)++;
}

/* Changer l'implémentation du test de valeur des tokens */
int canAddKeywordToken(TOKEN_LIST *tokenList, union TOKEN_VALUE value) {
	if ((strcmp(value.value_str, "main") == 0) ||
		(strcmp(value.value_str, "int") == 0) ||
		(strcmp(value.value_str, "while") == 0) ||
		(strcmp(value.value_str, "if") == 0) ||
		(strcmp(value.value_str, "for") == 0)) {
		addToken(tokenList, KEYWORD, value);
		return 1;
	}

	return 0;
}

int isAtEnd(char** text) {
	return **text == '\0';
}

int isAtNewLine(char** text) {
	return **text == '\n';
}

int isString(char** text, int* lenToken, regex_t* regex) {
	regmatch_t pmatch[1];

	if (regexec(regex, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

/* Seulement entiers positifs pour l'instant */
int isNumber(char** text, int* lenToken, regex_t* regex) {
	regmatch_t pmatch[1];
	
	if (regexec(regex, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

int isCharac(char** text, int* lenToken, regex_t* regex) {
	regmatch_t pmatch[1];

	if (regexec(regex, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

char* getString(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);

	return string;
}

int getNumber(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);
	
	int number = atoi(string);
	
	free(string);
	return number;
}

char* getChar(char** text, int lenToken) {
	char* word = extractSubString(text, lenToken);

	return word;
}

/**
* @brief Consomme nb caractères de la string pointée par text.
*/
void advance(char** text, int nb) {
	column += nb;
	if (isAtNewLine(text)) {
		line++;
		column = 0;
	}
	(*text) += nb; /* /!\ Ordre des opérateurs : *text++ <=> *(text++) */
}

/**
* @brief Créer une liste de tokens à partir d'une chaine text et la stock dans tokenList
*/
void lexer(char** text, TOKEN_LIST *tokenList) {
	int lenToken = 0;
	/* Allocation sur la pile (variable locale).
	*  Avant, j'avais écrit : regexList* regexes; ceci allouait sur le tas sans initialiser avec calloc -> pointe nulle part.
	*/
	regexList regexes;
	initRegexes(&regexes);

	/* Analyse lexicale */
	while (!isAtEnd(text)) {
		if (**text == ' ' || **text == '\n' || **text == '\t') {
			advance(text, 1);
			continue;
		}

		union TOKEN_VALUE value = { 0 };
		switch (**text) {
			/* OPERATOR */
			case '+': value.value_str = "+"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '-': value.value_str = "-"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '=': value.value_str = "="; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '<': value.value_str = "<"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '>': value.value_str = ">"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '%': value.value_str = "%"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '!': value.value_str = "!"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			
			/* PONCTUATION */
			case ';': value.value_str = ";"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case ',': value.value_str = ","; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '.': value.value_str = "."; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '(': value.value_str = "("; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case ')': value.value_str = ")"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '}': value.value_str = "}"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '{': value.value_str = "{"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case ']': value.value_str = "]"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '[': value.value_str = "["; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;

			/* STRINGS + NUMBERS + IDENTIFIERS + KEYWORDS */
			default:
			{
				/* STRINGS */
				if (isString(text, &lenToken, &(regexes.string))) {
					advance(text, 1); /* Enlève le premier '"' */
					value.value_str = getString(text, lenToken - 2); /* -2 : récupère l'interieur de la chaine de caractères */
					addToken(tokenList, STRING, value);
					advance(text, lenToken); /* Consomme le reste de la string */
				}
				/* NUMBERS */
				else if (isNumber(text, &lenToken, &(regexes.number))) {
					value.value_int = getNumber(text, lenToken);
					addToken(tokenList, NUMBER, value);
					advance(text, lenToken);
				}
				/* IDENTIFIERS + KEYWORDS */
				else if (isCharac(text, &lenToken, &(regexes.charac))) {
					value.value_str = getChar(text, lenToken);
					if (!canAddKeywordToken(tokenList, value)){ /* KEYWORDS */
						/* IDENTIFIERS */
						addToken(tokenList, IDENTIFIER, value);
					}
					advance(text, lenToken);
				}
				else {
					printf("Unknown caracter: %c, at line %d, column %d.\n", **text, line, column);
					advance(text, 1);
				}
				break;
			}
		}
	}

	/* DEBUG */
	printf("Liste des tokens (%d tokens):\n\n", tokenList->indexToken);
	char* tokenType;
	for (int i = 0; i < tokenList->indexToken; i++) {
		TOKEN token = tokenList->tokens[i];
		switch (token.type) {
			case 0: tokenType = "OPERATOR"; break;
			case 1: tokenType = "KEYWORD"; break;
			case 2: tokenType = "IDENTIFIER"; break;
			case 3: tokenType = "PONCTUATION"; break;
			case 4: tokenType = "STRING"; break;
			case 5: tokenType = "NUMBER"; break;
		}
		if (token.type == 5) {
			printf("#%d, TYPE: %s, VALUE: %d, LINE: %d, COLUMN: %d\n", i, tokenType, token.value.value_int, token.line, token.column);
		}
		else {
			printf("#%d, TYPE: %s, VALUE: %s, LINE: %d, COLUMN: %d\n", i, tokenType, token.value.value_str, token.line, token.column);
		}
	}
	printf("\n");

	freeRegexes(&regexes);
}

/* TODO
	* compléter la liste des keywords (pas hardcoder)
	* enlever les static pour multithreading
	* mettre en regex les opérateurs ?
*/