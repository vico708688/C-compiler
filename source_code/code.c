#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"


static int column = 0;
static int line = 0;

/* Ajoute un caractère au token voulu */
void addToken(TOKEN_LIST *tokenList, enum TOKEN_TYPE type, union TOKEN_VALUE value) {
	TOKEN token = { .type = type, .value = value, .line = line, .column = column };
	
	if (tokenList->indexToken > tokenList->size) {
		int old = tokenList->size;
		tokenList->size *= 2;
		if (tokenList = reallocarray(tokenList, tokenList->size, sizeof(TOKEN)) == NULL) {
                        perror("Error reallocarray\n");
                        exit(1);
                }
		printf("Realloc:\n  * old_size: %d\n  * new_size: %d\n", old, tokenList->size);
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
* @brief: Consomme nb caractères de la string pointée par text.
*/
void advance(char** text, int nb) {
	column += nb;
	if (isAtNewLine(text)) {
		line++;
		column = 0;
	}
	(*text) += nb; /* /!\ Ordre des opérateurs : *text++ <=> *(text++) */
}

void lexer(char** text, TOKEN_LIST *tokenList) {
	int lenToken = 0;
	/* Allocation sur la pile (variable locale).
	*  Avant, j'avais écrit : regexList* regexes; ceci allouait sur le tas sans initialiser avec calloc -> pointe nulle part.
	*/
	regexList regexes;
	initRegexes(&regexes);

	/* Analyse lexicale */
	printf("\nDébut de l'analyse lexicale...\n\n");
	while (!isAtEnd(text)) {
		if (**text == ' ' || **text == '\n' || **text == '\t') {
			advance(text, 1);
			continue;
		}

		union TOKEN_VALUE value = { 0 };
		switch (**text) {
			/* OPERATOR : mettre en regex ? */
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
					printf("String : %s\n", value.value_str);
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

	printf("\nAnalyse lexicale terminée.\n");
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

TOKEN_LIST initTokenList(char** text) {
	int lenOfText = lenText(text);
	/* Allocation de 256 tokens puis realloc */
	TOKEN_LIST list = { .size = 256 };
	
	if ((list.tokens = malloc(list.size * sizeof(TOKEN))) == NULL) {
		perror("Error malloc\n");
		exit(1);
	}

	return list;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage : %s <file_name>\n", argv[0]);
		exit(0);
	}

	FILE* file = fopen(argv[1], "r");
	char* text = read_file(file);
	char* backup_text = text; /* Modification de pointeur text dans lexer(), il faut donc sauvegarder le pointeur d'origine */

	TOKEN_LIST tokenList = initTokenList(&text);

	lexer(&text, &tokenList);

	freeTokens(&tokenList);
	free(backup_text);

	return 0;
}

/* TODO
	* compléter la liste des keywords (pas hardcoder)
	* allocation dynamique tokenList
	* enlever les static pour multithreading
*/