#ifndef H_UTILS
#define H_UTILS

int lenText(char** text);
char* read_file(FILE* fd);

void freeTokens(TOKEN_LIST* tokenList);
void freeRegexes(regex_t* regex);
void freeAll(TOKEN_LIST* tokenList, regex_t* regex);

char* extractSubString(char** string, int len);

#endif