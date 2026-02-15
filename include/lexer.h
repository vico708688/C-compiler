#ifndef H_LEXER
#define H_LEXER

typedef enum {
	/* Operators */
	OPERATOR,
	/* Keyword */
	KEYWORD,
    /* Identifier */
    IDENTIFIER,
	/* Ponctuation */
	PONCTUATION,
	/* Literals */
	LITERAL
} TOKEN_TYPE;

typedef union {
        char* value_str;
        int value_int;
} VALUE_UNION;

typedef struct {
	TOKEN_TYPE type;
	VALUE_UNION value;
	int line;
	int column;
} TOKEN;

typedef struct {
	TOKEN* tokens;
    int size;
} TOKEN_LIST;

void addToken(TOKEN_LIST *tokenList, TOKEN_TYPE type, VALUE_UNION value);
int canAddKeywordToken(TOKEN_LIST *tokenList, VALUE_UNION value);

int isAtEnd(char** text);
int isAtNewLine(char** text);
int isString(char** text, int* lenToken);
int isNumber(char** text, int* lenToken);
int isChar(char** text, int* lenToken);

char* getString(char** text, int lenToken);
char* getNumber(char** text, int lenToken);
char* getChar(char** text, int lenToken);

void lexer(char** text, TOKEN_LIST *tokenList);

void advance(char** text, int nb);
TOKEN_LIST initTokenList(char** text);

#endif