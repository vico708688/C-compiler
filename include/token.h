#ifndef H_TOKEN
#define H_TOKEN

enum TOKEN_TYPE {
	OPERATOR,
	KEYWORD,
    IDENTIFIER,
	PONCTUATION,
	STRING,
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

#endif