#ifndef H_PARSER
#define H_PARSER

#include "token.h"
#include "stdbool.h"
#include "AST.h"

TOKEN showNextToken(TOKEN_LIST* tokenList);
TOKEN acceptToken(TOKEN_LIST* tokenList);
TOKEN expectToken(TOKEN_LIST* tokenList, enum TOKEN_TYPE tokenType);

Program* parseProgram(TOKEN_LIST* tokenList);
Decl* parseDeclaration(TOKEN_LIST *tokenList);
Stmt* parseStatement(TOKEN_LIST *tokenList);
Asmt* parseAssignment(TOKEN_LIST *tokenList);
IfStmt* parseIfStatement(TOKEN_LIST *tokenList);
ElseStmt* parseElseStatement(TOKEN_LIST *tokenList);
WhileStmt* parseWhileStatement(TOKEN_LIST *tokenList);
Expr* parseExpression(TOKEN_LIST *tokenList);
Conj* parseConjunction(TOKEN_LIST *tokenList);
Equal* parseEquality(TOKEN_LIST *tokenList);
Relation* parseRelation(TOKEN_LIST *tokenList);
Addition* parseAddition(TOKEN_LIST *tokenList);
Term* parseTerm(TOKEN_LIST *tokenList);
Factor* parseFactor(TOKEN_LIST *tokenList);
Primary* parsePrimary(TOKEN_LIST *tokenList);
Parenth* parseParenth(TOKEN_LIST *tokenList);

void parser(TOKEN_LIST* tokenList);

#endif