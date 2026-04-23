#ifndef H_UTILS
#define H_UTILS

#include "token.h"
#include <regex.h>
#include <stdio.h>

typedef struct {
    regex_t string;
    regex_t integer;
    regex_t floating;
    regex_t comment;
    regex_t charac;
} regexList;

char* read_file(FILE* fd);

void freeTokens(TOKEN_LIST* tokenList);
void freeRegexes(regexList* regexes);

void initRegexes(regexList* regexes);
TOKEN_LIST *initTokenList(char** text);

char* extractSubString(char** string, int len);

#endif