#ifndef H_VISITOR
#define H_VISITOR

#include "AST.h"

typedef struct Visitor Visitor;

struct Visitor {
    void (*visitProgram)(Visitor*, Program*);
    void (*visitDecl)(Visitor*, Decl*);
    void (*visitStmt)(Visitor*, Stmt*);
    void (*visitExpr)(Visitor*, Expr*);

    int indent;
};

Visitor createPrettyPrinter();

#endif