#ifndef H_LEXER
#define H_LEXER

#include "token.h"

#include <stdbool.h>
#include <regex.h>

void addToken(TOKEN_LIST *tokenList, enum type_t type, union TOKEN_VALUE value, bool ownstr);
bool canAddKeywordToken(TOKEN_LIST *tokenList, union TOKEN_VALUE value);

bool isAtEnd(char** text);
bool isAtNewLine(char** text);

bool isType(char** text, int* lenToken, regex_t* regex);

int getInteger(char** text, int lenToken);
float getFloat(char** text, int lenToken);
char* getChar(char** text, int lenToken);

void lexer(char** text, TOKEN_LIST *tokenList);

void advance(char** text, int nb);

#endif