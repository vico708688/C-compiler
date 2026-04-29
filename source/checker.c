#include "checker.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Explore les assignements de variables
*/
static void visitExpr(Visitor* v, Expr* expr, Symtable* st) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_BINARY:
            visitExpr(v, expr->binary.left);
            visitExpr(v, expr->binary.right);
            break;

        case EXPR_UNARY:
            visitExpr(v, expr->unary.operand);
            break;

        case EXPR_IDENTIFIER:
            printf("%s", expr->identifier.name);
            break;

        case EXPR_ARRAY:
            visitExpr(v, expr->array.index);
            break;

        case EXPR_LITERAL:
            switch (expr->literal.value_type) {
                case TK_INT:
                    break;
                case TK_FLOAT:
                    break;
                case TK_BOOL:
                    break;
                case TK_CHAR:
                    break;
                default:
                    printf("Unknown literal.\n");
            }
            break;
    }    
}

/**
 * @brief Explore les déclarations de variables
*/
static void visitDecl(Visitor* v, Decl* decl, Symtable* st) {
    return;
}

/**
 * @brief Explore les utilisations de variables
*/
static void visitStmt(Visitor* v, Stmt* stmt, Symtable* st) {
    return;
}

/* --------------------- VISITOR ----------------------- */

Visitor createChecker() {
    Visitor v;

    v.visitProgram = visitProgram;
    v.visitDecl = visitDecl;
    v.visitStmt = visitStmt;
    v.visitExpr = visitExpr;

    v.indent = 0;

    return v;
}

static Symtable* createSymtable() {
    Symtable* st;
    
    st = malloc(sizeof(Symtable));

    if (st == NULL) {
        printf("Error st malloc.\n");
        exit(1);
    }

    st->name = "";
    st->symKind = VARIABLE;
    st->type = TK_INT;
    st->link = NULL;

    return st;
}

static void visitProgram(Visitor* v, Program* program) {
    Symtable* st = createSymtable();

    visitDecl(v, program->decl, st);
}