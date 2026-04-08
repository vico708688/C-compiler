#ifndef H_TOKEN
#define H_TOKEN

#include <stdbool.h>

typedef enum type_t {
	// Comments
	COMMENT,
	// Variable type
	TK_INT,
	TK_BOOL,
	TK_FLOAT,
	TK_CHAR,
	TK_ARRAY,
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
} type_t;

union TOKEN_VALUE {
	char* value_str;
	char value_chr;
	int value_int;
	float value_float;
	bool value_bool;
};

typedef struct {
	enum type_t type;
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