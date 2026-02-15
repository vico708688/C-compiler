#ifndef H_UTILS
#define H_UTILS

typedef struct {
    regex_t string;
    regex_t number;
    regex_t charac;
} regexList;

int lenText(char** text);
char* read_file(FILE* fd);

void freeTokens(TOKEN_LIST* tokenList);
void freeRegexes(regexList* regexes);

void initRegexes(regexList* regexes);

char* extractSubString(char** string, int len);

#endif