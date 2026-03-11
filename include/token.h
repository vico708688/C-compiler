#ifndef H_TOKEN
#define H_TOKEN

#include <stdbool.h>

enum TOKEN_TYPE {
	// Comments
	COMMENT,
	// Variable type
	TYPE_INT,
	TYPE_BOOL,
	TYPE_FLOAT,
	TYPE_CHAR,
	// Keywords
	KW_MAIN,
	KW_FALSE,
	KW_TRUE,
	KW_IF,
	KW_ELSE,
	KW_WHILE,
	// Comparisons
	EQUALITY,
	INEQUALITY,
	LT,
	LTE,
	GT,
	GTE,
	OR,
	AND,
	// Operators
	ADDITION,
	SUBTRACTION,
	MULTIPLICATION,
	DIVISION,
	MODULO,
	NOT,
	// Special characters
	ASSIGN,
	TERMINATOR,
	L_CURL_BRACKET,
	R_CURL_BRACKET,
	L_SQ_BRACKET,
	R_SQ_BRACKET,
	L_PAREN,
	R_PAREN,
	// Identifiers & Integers
	IDENTIFIER,
	INT_NUMBER,
	STRING,
	FLOAT_NUMBER
};

union TOKEN_VALUE {
	char* value_str;
	char value_chr;
	int value_int;
	float value_float;
	bool value_bool;
};

typedef struct {
	enum TOKEN_TYPE type;
	union TOKEN_VALUE value;
	bool ownstr;
	int line;
	int column;
} TOKEN;

typedef struct {
	TOKEN* tokens;
    int size;
	int indexToken;
} TOKEN_LIST;

#endif