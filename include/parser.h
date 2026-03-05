#ifndef H_PARSER
#define H_PARSER

#include "token.h"
#include "stdbool.h"

// typedef enum VarKind {
//     TYPE_INT,
// 	TYPE_BOOL,
// 	TYPE_CHAR,
// } VarKind;

/*
Une déclaration de variable contient:
  - un type
  - un identifier
  (- une valeur)?
*/
// typedef struct VarDecl
// {
//     VarKind type;
//     bool is_value_present;
//     char *name;
//     union
//     {
//         struct {
//             VarKind type;
//             union
//             {
//                 int value;
//                 char value;
//             };
//         };
//     };
// } VarDecl;

/*
Un statement peux être soit:
  - un assignment
  - un if_statement
  - un while_statement
*/
// typedef struct Stmt
// {
//     StmtKind type;
//     union
//     {
//         Assignment *assign;
//         struct block
//         {
//             StmtDecl *stmts;
//             int count;
//         };
//         struct ifStmt
//         {
//             ExprDecl *if_condition;
//             StmtDecl *else_if_condition;
//             StmtDecl *body;
//         };
//         struct forStmt
//         {
//             ExprDecl *expr;
//             StmtDecl *body;
//         };
//         struct whileStmt
//         {
//             ExprDecl *expr;
//             StmtDecl *body;
//         };

        
//     };
// } StmtDecl;

// typedef struct
// {
//     DeclKind type;
//     union
//     {
//         FnDecl   *fn;
//         ExprDecl *expr;
//         StmtDecl *stmt;
//         VarDecl  *var;
//     };
    
// } Decl;


// typedef struct Program {
//     TYPE_INT,

//     // Decl decl;
//     // int declar_count;
//     VarDecl *var;
// } Program;


TOKEN showNextToken(TOKEN_LIST* tokenList);
void acceptToken(TOKEN_LIST* tokenList);
void expectToken(TOKEN_LIST* tokenList, enum TOKEN_TYPE tokenType);

void parseProgram(TOKEN_LIST* tokenList);
void parseDeclaration(TOKEN_LIST *tokenList);
void parseStatement(TOKEN_LIST *tokenList);
void parseAssignment(TOKEN_LIST *tokenList);
void parseIfStatement(TOKEN_LIST *tokenList);
void parseElseStatement(TOKEN_LIST *tokenList);
void parseWhileStatement(TOKEN_LIST *tokenList);
void parseExpression(TOKEN_LIST *tokenList);
void parseConjunction(TOKEN_LIST *tokenList);
void parseEquality(TOKEN_LIST *tokenList);
void parseRelation(TOKEN_LIST *tokenList);
void parseAddition(TOKEN_LIST *tokenList);
void parseTerm(TOKEN_LIST *tokenList);
void parseFactor(TOKEN_LIST *tokenList);
void parsePrimary(TOKEN_LIST *tokenList);
void parseParenth(TOKEN_LIST *tokenList);

void parser(TOKEN_LIST* tokenList);

#endif