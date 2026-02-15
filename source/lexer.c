#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"


static int column = 0;
static int line = 0;

static int nbToken = 0;

/* Ajoute un caractère au token voulu */
void addToken(TOKEN_LIST *tokenList, TOKEN_TYPE type, VALUE_UNION value) {
	TOKEN token = { .type = type, .value = value, .line = line, .column = column };
	

	tokenList->tokens[nbToken] = token; /* tester si il reste de la place dans la liste */
	nbToken++;
}

int canAddKeywordToken(TOKEN_LIST *tokenList, VALUE_UNION value) {
	if ((strcmp(value.value_str, "main") == 0) |
		(strcmp(value.value_str, "int") == 0) |
		(strcmp(value.value_str, "while") == 0) |
		(strcmp(value.value_str, "if") == 0) |
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

int isString(char** text, int* lenToken) {
	regex_t regexChar;
	regmatch_t pmatch[1];

	if(regcomp(&regexChar, "^\".*\"", REG_EXTENDED)) {
		perror("Error regcomp\n");
		exit(1);
	}

	if (regexec(&regexChar, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

char* getString(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);

	return string;
}

/* Seulement entiers positifs */
int isNumber(char** text, int* lenToken) {
	regex_t regex;
	regmatch_t pmatch[1];
	
	if(regcomp(&regex, "^[0-9]+", REG_EXTENDED)) {
		perror("Error regcomp\n");
		exit(1);
	}
	
	if (regexec(&regex, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

char* getNumber(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);

	int number = atoi(string);

	return string;
}

int isChar(char** text, int* lenToken) {
	regex_t regexChar;
	regmatch_t pmatch[1];

	if(regcomp(&regexChar, "^[[:alpha:]][a-zA-Z_0-9]*", REG_EXTENDED)) {
		perror("Error regcomp\n");
		exit(1);
	}

	if (regexec(&regexChar, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

char* getChar(char** text, int lenToken) {
	char* word = extractSubString(text, lenToken);

	return word;
}

void advance(char** text, int nb) {
	column += nb;
	if (isAtNewLine(text)) {
		line += nb;
		column = 0;
	}
	(*text) += nb; /* /!\ Ordre des opérateurs : *text++ <=> *(text++) */
}

void lexer(char** text, TOKEN_LIST *tokenList) {
	int lenToken = 0;

	/* Analyse lexicale */
	printf("\nDébut de l'analyse lexicale...\n\n");
	while (!isAtEnd(text)) {
		if (**text == ' ') {
			advance(text, 1);
			continue;
		}

		VALUE_UNION value = { 0 };
		switch (**text) {
			/* OPERATOR */
			case '+': value.value_str = "+"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '-': value.value_str = "-"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '=': value.value_str = "="; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '<': value.value_str = "<"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '>': value.value_str = ">"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '!': value.value_str = "!"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;
			case '%': value.value_str = "%"; addToken(tokenList, OPERATOR, value); advance(text, 1); break;

			/* PONCTUATION */
			case ';': value.value_str = ";"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case ',': value.value_str = ","; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '(': value.value_str = "("; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case ')': value.value_str = ")"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '}': value.value_str = "}"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '{': value.value_str = "{"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case ']': value.value_str = "]"; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;
			case '[': value.value_str = "["; addToken(tokenList, PONCTUATION, value); advance(text, 1); break;

			/* LITERALS + IDENTIFIERS + KEYWORDS */
			default:
			{
				if (isString(text, &lenToken)) {
					value.value_str = getString(text, lenToken);
					addToken(tokenList, LITERAL, value);
					advance(text, lenToken);
				}
				else if (isNumber(text, &lenToken)) {
					value.value_str = getNumber(text, lenToken);
					addToken(tokenList, LITERAL, value);
					advance(text, lenToken);
				}
				else if (isChar(text, &lenToken)) {
					value.value_str = getChar(text, lenToken);
					/* Si le mot est un keyword, on l'ajoute à la liste, sinon c'est un identifier */
					if (!canAddKeywordToken(tokenList, value)){
						addToken(tokenList, LITERAL, value);
					}
					advance(text, lenToken);
				}
				else {
					//printf("Unknown caracter: %c, at line %d, column %d.\n", **text, line, column);
					advance(text, 1);
				}
				break;
			}
		}
	}

	printf("\nAnalyse lexicale terminée.\n");
	printf("Liste des tokens (%d tokens):\n\n", nbToken);
	char* tokenType;
	for (int i = 0; i < nbToken; i++) {
		switch (tokenList->tokens[i].type) {
			case 0: tokenType = "OPERATOR"; break;
			case 1: tokenType = "KEYWORD"; break;
			case 2: tokenType = "IDENTIFIER"; break;
			case 3: tokenType = "PONCTUATION"; break;
			case 4: tokenType = "LITERAL"; break;
		}
		printf("#%d, TYPE: %s, VALUE: %s, LINE: %d, COLUMN: %d\n", i, tokenType, tokenList->tokens[i].value.value_str, tokenList->tokens[i].line, tokenList->tokens[i].column);
	}
	printf("\n");
}

TOKEN_LIST initTokenList(char** text) {
	int lenOfText = lenText(text);
	TOKEN_LIST list = { .size = 2 * lenOfText };
	
	if ((list.tokens = malloc(list.size * sizeof(TOKEN))) == NULL) { /* Facteur 2 pris au hazard */
		perror("Error malloc\n");
		exit(1);
	}
	printf("Nb estimé de tokens : %d\n", list.size);
	
	return list;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage : %s <file_name>\n", argv[0]);
		exit(0);
	}

	FILE* file = fopen(argv[1], "r");
	char* text = read_file(file);

	TOKEN_LIST tokenList = initTokenList(&text);

	lexer(&text, &tokenList);

	freeTokens(&tokenList);

	return 0;
}

/* TODO
	* ne pas recalculer les regex à chaque fois
	* fonctionnement union avec le value_str et value_int (condition sur le type ?)
	* compléter la liste des keywords
*/