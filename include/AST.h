#ifndef H_AST
#define H_AST

#include "stdbool.h"
#include "token.h"

/* --------------------------------------- Forward Declaration --------------------------------------- */

typedef struct Decl Decl;
typedef struct ElseStmt ElseStmt;
typedef struct IfStmt IfStmt;
typedef struct Parenth Parenth;
typedef struct Primary Primary;
typedef struct Factor Factor;
typedef struct Term Term;
typedef struct Addition Addition;
typedef struct Relation Relation;
typedef struct Equal Equal;
typedef struct Conj Conj;
typedef struct Expr Expr;
typedef struct Asmt Asmt;
typedef struct Stmt Stmt;
typedef struct Program Program;

/* --------------------------------------- Définition des types -------------------------------------- */

typedef enum AST_Equop
{
    AST_EQUALITY,
    AST_INEQUALITY
} AST_Equop;

typedef enum AST_Relop
{
    AST_LT,
	AST_LTE,
	AST_GT,
	AST_GTE
} AST_Relop;

typedef enum AST_Addop
{
    AST_ADDITION,
	AST_SUBTRACTION
} AST_Addop;

typedef enum AST_Mulop
{
    AST_MULTIPLICATION,
	AST_DIVISION,
	AST_MODULO
} AST_Mulop;

typedef enum AST_Unaryop
{
    AST_MINUS,
    AST_NOT
} AST_Unaryop;

typedef enum Stmt_type
{
    ASSIGNMENT,
    IF_STATEMENT,
    WHILE_STATEMENT
} Stmt_type;

typedef enum Primary_type
{
    PRIMARY_IDENTIFIER,
    PRIMARY_ARRAY_ACCESS,
    PRIMARY_LITERAL,
    PRIMARY_PARENTHESIZED_EXPRESSION
} Primary_type;

/* ------------------------------------ Définition de la grammaire ------------------------------------*/

typedef struct Decl
{
    enum TOKEN_TYPE type;
    char* id;
    bool is_array;
    int length;
} Decl;

typedef struct ElseStmt
{
    int nb_stmt;
    Stmt** stmt;
} ElseStmt;

typedef struct IfStmt
{
    Expr* expr;
    int nb_stmt;
    Stmt** stmt;
    bool contains_elseStmt;
    ElseStmt* else_stmt;
} IfStmt;

typedef struct WhileStmt
{
    Expr* expr;
    int nb_stmt;
    Stmt** stmt;
} WhileStmt;

typedef struct Parenth
{
    Expr* expr;
} Parenth;

typedef struct Primary
{
    enum Primary_type type;
    union
    {
        struct
        {
            char* id;
            Expr* expr;
        } identifier;
        enum TOKEN_TYPE value_type;
        union TOKEN_VALUE value;
        Parenth* parent;
    };
} Primary;

typedef struct Factor
{
    bool contains_unaryop;
    AST_Unaryop unaryop;
    Primary* primary;
} Factor;

typedef struct Term
{
    Factor* factor;
    AST_Mulop* mulop;
    int nb_factor;
    Factor** rfactor;
} Term;

typedef struct Addition
{
    Term* term;
    AST_Addop* addop;
    int nb_term;
    Term** rterm;
} Addition;

typedef struct Relation
{
    Addition* laddition;
    bool contains_laddition;
    AST_Relop relop;
    Addition* raddition;
} Relation;

typedef struct Equal
{
    Relation* lrelation;
    bool contains_lequal;
    AST_Equop equop;
    Relation* rrelation;
} Equal;

typedef struct Conj
{
    Equal* lequal;
    int nb_requal;
    Equal** requal;
} Conj;

typedef struct Expr
{
    Conj* lconj;
    int nb_rconj;
    Conj** rconj;
} Expr;

typedef struct Asmt
{
    char* id;
    bool contains_lexpr;
    Expr* lexpr;
    Expr* rexpr;
} Asmt;

typedef struct Stmt
{
    enum Stmt_type type;
    union
    {
        Asmt* asmt;
        IfStmt* if_stmt;
        WhileStmt* while_stmt;
    };
} Stmt;

typedef struct Program
{
    int nb_decl;
    int nb_stmt;
    Decl** decl;
    Stmt** stmt;
} Program;

#endif