#include "visitor.h"

#include <stdio.h>

static void visitExpr(Visitor* v, Expr* expr);
static void visitStmt(Visitor* v, Stmt* stmt);
static void visitDecl(Visitor* v, Decl* decl);
static void visitProgram(Visitor* v, Program* program);

void printIndent(int indent) {
    for (int i = 0; i < indent; i++)
        printf("    ");
}

const char* tokenToString(enum type_t type) {
    switch (type) {
        case ADDITION: return "+";
        case SUBTRACTION: return "-";
        case MULTIPLICATION: return "*";
        case DIVISION: return "/";
        case MODULO: return "%";

        case EQUALITY: return "==";
        case INEQUALITY: return "!=";

        case LT: return "<";
        case LTE: return "<=";
        case GT: return ">";
        case GTE: return ">=";

        case AND: return "&&";
        case OR: return "||";
        case NOT: return "!";

        default: return "?";
    }
}

/* ------------------- EXPRESSIONS ------------------- */

static void visitExpr(Visitor* v, Expr* expr) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_BINARY:
            printf("(");
            visitExpr(v, expr->binary.left);
            printf(" %s ", tokenToString(expr->binary.op));
            visitExpr(v, expr->binary.right);
            printf(")");
            break;

        case EXPR_UNARY:
            if (expr->unary.got_op) {
                printf("%s", tokenToString(expr->unary.op));
            }
            visitExpr(v, expr->unary.operand);
            break;

        case EXPR_IDENTIFIER:
            printf("%s", expr->identifier.name);
            break;

        case EXPR_ARRAY:
            printf("%s[", expr->array.name);
            visitExpr(v, expr->array.index);
            printf("]");
            break;

        case EXPR_LITERAL:
            switch (expr->literal.value_type) {
                case TK_INT:
                    printf("%d", expr->literal.value.value_int);
                    break;
                case TK_FLOAT:
                    printf("%f", expr->literal.value.value_float);
                    break;
                case TK_BOOL:
                    printf(expr->literal.value.value_bool ? "true" : "false");
                    break;
                case TK_CHAR:
                    printf("\"%s\"", expr->literal.value.value_str);
                    break;
                default:
                    printf("?");
            }
            break;
    }
}

/* ------------------- STATEMENTS ------------------- */

static void visitStmt(Visitor* v, Stmt* stmt) {
    while (stmt) {
        printIndent(v->indent);

        switch (stmt->type) {
            case STMT_DECL:
                visitDecl(v, stmt->decl);
                break;

            case STMT_ASSIGN:
                printf("%s", stmt->asmt->id);

                if (stmt->asmt->contains_lexpr) {
                    printf("[");
                    visitExpr(v, stmt->asmt->lexpr);
                    printf("]");
                }

                printf(" = ");
                visitExpr(v, stmt->asmt->rexpr);
                printf(";\n");
                break;

            case STMT_IF_ELSE:
                printf("if (");
                visitExpr(v, stmt->expr);
                printf(") {\n");

                v->indent++;
                visitStmt(v, stmt->body);
                v->indent--;

                printIndent(v->indent);
                printf("}");

                if (stmt->else_body) {
                    printf(" else {\n");
                    v->indent++;
                    visitStmt(v, stmt->else_body);
                    v->indent--;

                    printIndent(v->indent);
                    printf("}");
                }

                printf("\n");
                break;

            case STMT_WHILE:
                printf("while (");
                visitExpr(v, stmt->expr);
                printf(") {\n");

                v->indent++;
                visitStmt(v, stmt->body);
                v->indent--;

                printIndent(v->indent);
                printf("}\n");
                break;

            default:
                printf("// unknown stmt\n");
        }

        stmt = stmt->next;
    }
}

/* ------------------- DECL ------------------- */

static void visitDecl(Visitor* v, Decl* decl) {
    if (!decl) return;

    switch (decl->type) {
        case TK_INT: printf("int "); break;
        case TK_FLOAT: printf("float "); break;
        case TK_BOOL: printf("bool "); break;
        case TK_CHAR: printf("char "); break;
        default: printf("unknown "); break;
    }

    printf("%s", decl->id);

    if (decl->code) {
        printf("() {\n");
        v->indent++;
        visitStmt(v, decl->code);
        v->indent--;
        printf("}\n");
    } else {
        printf(";\n");
    }
}

/* ------------------- PROGRAM ------------------- */

static void visitProgram(Visitor* v, Program* program) {
    visitDecl(v, program->decl);
}

/* ------------------- FACTORY ------------------- */

Visitor createPrettyPrinter() {
    Visitor v;

    v.visitProgram = visitProgram;
    v.visitDecl = visitDecl;
    v.visitStmt = visitStmt;
    v.visitExpr = visitExpr;

    v.indent = 0;

    return v;
}