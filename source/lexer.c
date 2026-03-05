#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

static int column = 0;
static int line = 0;

void addToken(TOKEN_LIST *tokenList, enum TOKEN_TYPE type, union TOKEN_VALUE value, bool ownstr) {
	TOKEN token = { .type = type, .value = value, .line = line, .column = column, .ownstr = ownstr };
	
	if (tokenList->indexToken >= tokenList->size) {
		TOKEN *tmp = NULL;
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

bool canAddKeywordToken(TOKEN_LIST *tokenList, union TOKEN_VALUE value) {
	if (strcmp(value.value_str, "int") == 0) {
		addToken(tokenList, TYPE_INT, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "bool") == 0) {
		addToken(tokenList, TYPE_BOOL, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "float") == 0) {
		addToken(tokenList, TYPE_FLOAT, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "char") == 0) {
		addToken(tokenList, TYPE_CHAR, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "main") == 0) {
		addToken(tokenList, KW_MAIN, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "while") == 0) {
		addToken(tokenList, KW_WHILE, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "if") == 0) {
		addToken(tokenList, KW_IF, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "else") == 0) {
		addToken(tokenList, KW_ELSE, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "false") == 0) {
		addToken(tokenList, KW_FALSE, value, true);
		return true;
	}
	else if (strcmp(value.value_str, "true") == 0) {
		addToken(tokenList, KW_TRUE, value, true);
		return true;
	}
	return false;
}

bool isAtEnd(char** text) {
	return **text == '\0';
}

bool isAtNewLine(char** text) {
	return **text == '\n';
}

bool isType(char** text, int* lenToken, regex_t* regex) {
	regmatch_t pmatch[1];
	
	if (regexec(regex, *text, 1, pmatch, 0) == 0) {
		*lenToken = pmatch[0].rm_eo;
		return 1;
	}

	return 0;
}

int getInteger(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);
	
	int number = atoi(string);
	
	free(string);
	return number;
}

float getFloat(char** text, int lenToken) {
	char* string = extractSubString(text, lenToken);
	
	float number = atof(string);
	
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
		bool ownstr = false;
		switch (**text) {
			case '+': {
				value.value_chr = '+';
				addToken(tokenList, ADDITION, value, false);
				advance(text, 1);
				break;
			}
			case '-': {
				value.value_chr = '-';
				addToken(tokenList, SUBTRACTION, value, false);
				advance(text, 1);
				break;
			}
			case '*': {
				value.value_chr = '*';
				addToken(tokenList, MULTIPLICATION, value, false);
				advance(text, 1);
				break;
			}
			case '/': {
				value.value_chr = '/';
				addToken(tokenList, DIVISION, value, false);
				advance(text, 1);
				break;
			}
			case '=': {
				bool is_equal = ((*text)[1] == '=');

				if (is_equal)
					value.value_str = "==";
				else
					value.value_chr = '=';

				addToken(tokenList,
						is_equal ? EQUALITY : ASSIGN,
						value,
						ownstr);

				advance(text, is_equal ? 2 : 1);
				break;
			}
			case '<': {
				bool is_lte = ((*text)[1] == '=');

				if (is_lte)
					value.value_str = "<=";
				else
					value.value_chr = '<';

				addToken(tokenList,
						is_lte ? LTE : LT,
						value,
						ownstr);

				advance(text, is_lte ? 2 : 1);
				break;
			}
			case '>': {
				bool is_gte = ((*text)[1] == '=');

				if (is_gte)
					value.value_str = ">=";
				else
					value.value_chr = '>';

				addToken(tokenList,
						is_gte ? GTE : GT,
						value,
						ownstr);

				advance(text, is_gte ? 2 : 1);
				break;
			}
			case '%': {
				value.value_chr = '%';
				addToken(tokenList, MODULO, value, false);
				advance(text, 1);
				break;
			}
			case '!': {
				bool is_ne = ((*text)[1] == '=');

				if (is_ne)
					value.value_str = "!=";
				else
					value.value_chr = '!';

				addToken(tokenList,
						is_ne ? INEQUALITY : NOT,
						value,
						ownstr);

				advance(text, is_ne ? 2 : 1);
				break;
			}

			/* PONCTUATION */
			case ';':
				value.value_chr = **text;
				addToken(tokenList, TERMINATOR, value, false);
				advance(text, 1);
				break;
			case '{':
				value.value_chr = **text;
				addToken(tokenList, L_CURL_BRACKET, value, false);
				advance(text, 1);
				break;
			case '}':
				value.value_chr = **text;
				addToken(tokenList, R_CURL_BRACKET, value, false);
				advance(text, 1);
				break;
			case '[':
				value.value_chr = **text;
				addToken(tokenList, L_SQ_BRACKET, value, false);
				advance(text, 1);
				break;
			case ']':
				value.value_chr = **text;
				addToken(tokenList, R_SQ_BRACKET, value, false);
				advance(text, 1);
				break;
			case '(':
				value.value_chr = **text;
				addToken(tokenList, L_PAREN, value, false);
				advance(text, 1);
				break;
			case ')':
				value.value_chr = **text;
				addToken(tokenList, R_PAREN, value, false);
				advance(text, 1);
				break;

			/* STRINGS + NUMBERS + IDENTIFIERS + KEYWORDS */
			default:
			{
				/* FLOATS */
				if (isType(text, &lenToken, &(regexes.floating))) {
					value.value_float = getFloat(text, lenToken);
					addToken(tokenList, FLOAT_NUMBER, value, false);
					advance(text, lenToken);
				}/* INTEGERS */
				else if (isType(text, &lenToken, &(regexes.integer))) {
					value.value_int = getInteger(text, lenToken);
					addToken(tokenList, INT_NUMBER, value, false);
					advance(text, lenToken);
				}
				/* IDENTIFIERS + KEYWORDS */
				else if (isType(text, &lenToken, &(regexes.charac))) {
					value.value_str = getChar(text, lenToken);
					if (!canAddKeywordToken(tokenList, value)){ /* KEYWORDS */
						/* IDENTIFIERS */
						addToken(tokenList, IDENTIFIER, value, true);
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

	#ifdef DEBUG
	printf("Liste des tokens (%d tokens):\n\n", tokenList->indexToken);
	for (int i = 0; i < tokenList->indexToken; i++) {
		TOKEN token = tokenList->tokens[i];
		if (token.ownstr) {
			printf("#%d, TYPE: %d, VALUE: %s, LINE: %d, COLUMN: %d\n", i, token.type, token.value.value_str, token.line, token.column);
		}
		else if (token.type <= 32) {
			printf("#%d, TYPE: %d, VALUE: %s, LINE: %d, COLUMN: %d\n", i, token.type, (char[]){token.value.value_chr, '\0'}, token.line, token.column);
		}
		else if (token.type == 34) {
			printf("#%d, TYPE: %d, VALUE: %d, LINE: %d, COLUMN: %d\n", i, token.type, token.value.value_int, token.line, token.column);
		}
		else {
			printf("#%d, TYPE: %d, VALUE: %f, LINE: %d, COLUMN: %d\n", i, token.type, token.value.value_float, token.line, token.column);
		}
	}
	printf("\n");
	#endif

	freeRegexes(&regexes);
}