#ifndef H_LEXER
#define H_LEXER

#include "token.h"
#include <regex.h>

void addToken(TOKEN_LIST *tokenList, enum TOKEN_TYPE type, union TOKEN_VALUE value);
int canAddKeywordToken(TOKEN_LIST *tokenList, union TOKEN_VALUE value);

int isAtEnd(char** text);
int isAtNewLine(char** text);

int isString(char** text, int* lenToken, regex_t* regex);
int isNumber(char** text, int* lenToken, regex_t* regex);
int isCharac(char** text, int* lenToken, regex_t* regex);

char* getString(char** text, int lenToken);
int getNumber(char** text, int lenToken);
char* getChar(char** text, int lenToken);

void lexer(char** text, TOKEN_LIST *tokenList);

void advance(char** text, int nb);

#endif