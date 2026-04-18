#ifndef H_PARSER
#define H_PARSER

#include "token.h"
#include "stdbool.h"
#include "AST.h"

Program* parser(TOKEN_LIST* tokenList);

TOKEN showNextToken(TOKEN_LIST* tokenList);
TOKEN acceptToken(TOKEN_LIST* tokenList);
TOKEN expectToken(TOKEN_LIST* tokenList, enum type_t tokenType);

// ------------------------------------------- Program parsing ----------------------------------------------
Program* parseProgram(TOKEN_LIST* tokenList);
Decl* decl_create(enum type_t type, char* id, Stmt* code);

// ------------------------------------------- Expression parsing -------------------------------------------
Expr* parseExpression(TOKEN_LIST *tokenList);

Expr* parseConjunction(TOKEN_LIST *tokenList);
Expr* parseEquality(TOKEN_LIST *tokenList);
Expr* parseRelation(TOKEN_LIST *tokenList);
Expr* parseAddition(TOKEN_LIST *tokenList);
Expr* parseTerm(TOKEN_LIST *tokenList);
Expr* parseFactor(TOKEN_LIST *tokenList);
Expr* parsePrimary(TOKEN_LIST *tokenList);
Expr* parseParenth(TOKEN_LIST *tokenList);

// ------------------------------------------- Statement parsing --------------------------------------------
Stmt* stmt_create(stmt_t kind, Decl *decl, Asmt* asmt, Expr *init_expr, Expr *expr, Expr *next_expr, Stmt *body, Stmt *else_body, Stmt *next);
Stmt* parseStatement(TOKEN_LIST *tokenList);

Decl* parseDeclaration(TOKEN_LIST *tokenList);
IfStmt* parseIfStatement(TOKEN_LIST *tokenList);
WhileStmt* parseWhileStatement(TOKEN_LIST *tokenList);
Asmt* parseAssignment(TOKEN_LIST *tokenList);

// -------- DEBUG ----------
void printIndent(int indent);
void printExpr(Expr* expr, int indent);
void printStmt(Stmt* stmt, int indent);
void printProgram(Program* program);

#endif