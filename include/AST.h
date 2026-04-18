#ifndef H_AST
#define H_AST

#include "stdbool.h"
#include "token.h"

// Statement :
//      Body of a function.
//      Indicates that the program is to take a particular action in the order speci-
//      fied, such as computing a value, performing a loop, or choosing between
//      branches of an alternative. A statement can also be a declaration of a local
//      variable.
// Declaration :
//      States the existence of a variable or a function
// Expression :
//      Assignement d'une valeur

typedef struct Stmt Stmt;

// -------------------------------------- Expression structure --------------------------------------
typedef enum {
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_LITERAL,
    EXPR_IDENTIFIER,
    EXPR_ARRAY
} ExprType;

typedef struct Expr Expr;

struct Expr {
    ExprType type;

    union {
        struct {
            Expr* left;
            Expr* right;
            enum type_t op;
        } binary;

        struct {
            bool got_op;
            enum type_t op;
            Expr* operand;
        } unary;

        struct {
            char* name;
        } identifier;

        struct {
            char* name;
            Expr* index;
        } array;

        struct {
            enum type_t value_type;
            union TOKEN_VALUE value;
        } literal;
    };
};

// ------------------------------------ Declaration structure ---------------------------------------
typedef struct {
    enum type_t type;
    char* id;
    Stmt* code; // Only used by functions
} Decl;

// -------------------------------------- Statement structure ---------------------------------------
typedef enum {
    STMT_DECL,
    STMT_ASSIGN,
    STMT_IF_ELSE,
    STMT_WHILE,
    STMT_BLOCK,
    STMT_RETURN
} stmt_t;

typedef struct {
    char* id;
    bool contains_lexpr;
    Expr* lexpr;
    Expr* rexpr;
} Asmt;

typedef struct {
    Expr* condition;
    Stmt* body;
} WhileStmt;

typedef struct {
    Expr* condition;
    Stmt* then_branch;
    bool contains_else_stmt;
    Stmt* else_branch;
} IfStmt;

struct Stmt {
    stmt_t type;
    Decl* decl;
    Asmt* asmt;
    Expr* init_expr; // Used for "for loop"
    Expr* expr;      // Used for "for loop" and "while loop" and "if/else loop"
    Expr* next_expr; // Used for "for loop"
    Stmt* body;      // Used for "for loop" and "while loop" and "if/else loop"
    Stmt* else_body; // Used for "if/else loop"
    Stmt* next;
};

// -------------------------------------- Program structure -----------------------------------------
typedef struct {
    Decl* decl;
} Program;

#endif