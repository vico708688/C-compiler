#ifndef H_PRETTY_PRINTER
#define H_PRETTY_PRINTER

#include "AST.h"

typedef struct Visitor Visitor;

struct Visitor {
    void (*visitProgram)(Visitor*, Program*);
    void (*visitDecl)(Visitor*, Decl*);
    void (*visitStmt)(Visitor*, Stmt*);
    void (*visitExpr)(Visitor*, Expr*);

    int indent;
};

static void visitExpr(Visitor* v, Expr* expr);
static void visitStmt(Visitor* v, Stmt* stmt);
static void visitDecl(Visitor* v, Decl* decl);
static void visitProgram(Visitor* v, Program* program);

Visitor createPrettyPrinter();

#endif