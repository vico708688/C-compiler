#ifndef H_CHECKER
#define H_CHECKER

#include "AST.h"

typedef struct Visitor Visitor;
typedef struct Symtable Symtable;

enum symbolKind {
    FUNCTION,
    VARIABLE,
    PARAMETER
};

struct Symtable {
    char* name;
    enum symbolKind symKind;
    enum type_t type;
    Symtable* link;
};

struct Visitor {
    void (*visitProgram)(Visitor*, Program*);
    void (*visitDecl)(Visitor*, Decl*, Symtable*);
    void (*visitStmt)(Visitor*, Stmt*, Symtable*);
    void (*visitExpr)(Visitor*, Expr*, Symtable*);

    int indent;
};

static void visitExpr(Visitor* v, Expr* expr, Symtable* st);
static void visitStmt(Visitor* v, Stmt* stmt, Symtable* st);
static void visitDecl(Visitor* v, Decl* decl, Symtable* st);
static void visitProgram(Visitor* v, Program* program);

Visitor createChecker();

#endif