#ifndef H_LEXER
#define H_LEXER

enum TOKEN_TYPE {
	/* Operators */
	OPERATOR,
	/* Keyword */
	KEYWORD,
    /* Identifier */
    IDENTIFIER,
	/* Ponctuation */
	PONCTUATION,
	/* String */
	STRING,
	/* Number */
	NUMBER
};

union TOKEN_VALUE {
        char* value_str;
        int value_int;
};

typedef struct {
	enum TOKEN_TYPE type;
	union TOKEN_VALUE value;
	int line;
	int column;
} TOKEN;

typedef struct {
	TOKEN* tokens;
    int size;
	int indexToken;
} TOKEN_LIST;

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
TOKEN_LIST initTokenList(char** text);

#endif