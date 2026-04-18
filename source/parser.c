#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "visitor.h"

#ifdef DEBUG
int tabs = 0;
#endif

/**
* @brief Retourne le k-ième token de la liste sans le consommer.
*/
TOKEN showNextToken(TOKEN_LIST* tokenList) {
    return tokenList->tokens[tokenList->indexToken];
}

/**
* @brief Consomme le token courant en l'enlevant de la liste.
*/
TOKEN acceptToken(TOKEN_LIST* tokenList) {
    tokenList->indexToken++;
    return tokenList->tokens[tokenList->indexToken - 1];
}

/**
* @brief consomme le lexème courant s’il est du bon type (kind). Sinon
* retourne une erreur à l’utilisateur en indiquant la ligne et la colonne d’erreur.
*/
TOKEN expectToken(TOKEN_LIST* tokenList, enum type_t tokenType) {
    TOKEN token = showNextToken(tokenList);
    if (token.type != tokenType) {
        printf("Syntax error line %d, column %d. Expected %d, got %d.\n", token.line, token.column, tokenType, token.type);
        exit(1); /* A enlever : correction d'erreurs multiples */
    }
    return acceptToken(tokenList);
}

/**
* @brief Analyse récursive descendante LL(1).
*/
Program* parser(TOKEN_LIST* tokenList) {
    /* Initialise à 0 l'indice du token courant */
    tokenList->indexToken = 0;

    /* TODO : remove comments */
    Program *program = parseProgram(tokenList);

    return program;
}

// ------------------------------------ Méthodes de parsing ----------------------------------------
#ifdef DEBUG
void printDebug(char* string) {
    for (int i = 0; i < tabs; i++) {
        printf("   ");
    }
    printf("%s\n", string);
}
#endif

// ------------------------------------------- Program parsing ----------------------------------------------
Program* parseProgram(TOKEN_LIST* tokenList) {
    Program *program = malloc(sizeof(Program));
    if (program == NULL)
    {
        perror("Error program malloc.\n");
        exit(EXIT_FAILURE);
    }
    
    #ifdef DEBUG
    printDebug("Entering program");
    tabs++;
    #endif
    expectToken(tokenList, TK_INT);
    expectToken(tokenList, KW_MAIN);
    expectToken(tokenList, L_PAREN);
    expectToken(tokenList, R_PAREN);
    expectToken(tokenList, L_CURL_BRACKET);
    
    Decl *main_func = decl_create(TK_INT, "main", NULL);
    program->decl = main_func;

    Stmt* head = NULL;        // début de la liste
    Stmt* tail = NULL;        // fin de la liste
    
    enum type_t nextTokenType = showNextToken(tokenList).type;   

    while (nextTokenType != R_CURL_BRACKET) {
        Stmt* new_stmt = parseStatement(tokenList);
        new_stmt->next = NULL;

        if (head == NULL) // initialisation de la liste chainée
        {
            head = new_stmt;
            tail = new_stmt;
        }
        else
        {
            tail->next = new_stmt;
            tail = new_stmt;
        }

        nextTokenType = showNextToken(tokenList).type;
    }
    expectToken(tokenList, R_CURL_BRACKET);

    main_func->code = head;

    #ifdef DEBUG
    printf("\n");
    tabs--;
    #endif

    return program;
}

Decl* decl_create(enum type_t type, char* id, Stmt* code) {
    Decl* decl = malloc(sizeof(Decl));
    decl->type = type;
    decl->id = id;
    decl->code = code;
    return decl;
}

// ------------------------------------------- Expression parsing ----------------------------------------------
Expr* newBinaryExpr(Expr* left, enum type_t op, Expr* right) {
    Expr *expr = malloc(sizeof(Expr));
    if (expr == NULL)
    {
        perror("Error binary expression malloc.\n");
        exit(EXIT_FAILURE);
    }
    expr->type = EXPR_BINARY;
    expr->binary.left = left;
    expr->binary.op = op;
    expr->binary.right = right;
    return expr;
}

Expr* newUnaryExpr(Expr* expression, enum type_t op) {
    Expr *expr = malloc(sizeof(Expr));
    if (expr == NULL)
    {
        perror("Error binary expression malloc.\n");
        exit(EXIT_FAILURE);
    }
    expr->type = EXPR_UNARY;
    expr->unary.op = op;
    expr->unary.operand = expression;
    return expr;
}

Expr* parseExpression(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing conjunction");
    tabs++;
    #endif
    Expr* e1 = parseConjunction(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif

    while (showNextToken(tokenList).type == OR) {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing conjunction");
        tabs++;
        #endif
        enum type_t op = OR;
        Expr* e2 = parseConjunction(tokenList);
        e1 = newBinaryExpr(e1, op, e2);
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return e1;
}

Expr* parseConjunction(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing equality");
    tabs++;
    #endif
    Expr* e1 = parseEquality(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif

    while (showNextToken(tokenList).type == AND) {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing equality");
        tabs++;
        #endif
        enum type_t op = AND;
        Expr* e2 = parseEquality(tokenList);
        e1 = newBinaryExpr(e1, op, e2);
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return e1;
}

Expr* parseEquality(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing relation");
    tabs++;
    #endif
    Expr* e1 = parseRelation(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    
    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == EQUALITY ||
        nextToken.type == INEQUALITY)
    {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing relation");
        tabs++;
        #endif
        Expr* e2 = parseRelation(tokenList);
        e1 = newBinaryExpr(e1, nextToken.type, e2);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    return e1;
}
    
Expr* parseRelation(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing addition");
    tabs++;
    #endif
    Expr* e1 = parseAddition(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    
    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == LT ||
        nextToken.type == LTE ||
        nextToken.type == GT ||
        nextToken.type == GTE)
    {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing addition");
        tabs++;
        #endif
        Expr* e2 = parseAddition(tokenList);
        e1 = newBinaryExpr(e1, nextToken.type, e2);
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return e1;
}

Expr* parseAddition(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing term");
    tabs++;
    #endif
    Expr* e1 = parseTerm(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif

    TOKEN nextToken = showNextToken(tokenList);
    while (nextToken.type == ADDITION ||
        nextToken.type == SUBTRACTION)
    {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing term");
        tabs++;
        #endif
        Expr* e2 = parseTerm(tokenList);
        e1 = newBinaryExpr(e1, nextToken.type, e2);
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }

    return e1;
}

Expr* parseTerm(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing factor");
    tabs++;
    #endif
    Expr* e1 = parseFactor(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif

    TOKEN nextToken = showNextToken(tokenList);
    while (nextToken.type == MULTIPLICATION ||
        nextToken.type == MODULO ||
        nextToken.type == DIVISION)
    {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing factor");
        tabs++;
        #endif
        Expr* e2 = parseTerm(tokenList);
        e1 = newBinaryExpr(e1, nextToken.type, e2);
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }

    return e1;
}

Expr* parseFactor(TOKEN_LIST* tokenList) {
    enum type_t op;
    bool has_op = false;

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == SUBTRACTION ||
        nextToken.type == NOT)
    {
        has_op = true;
        op = nextToken.type;
        acceptToken(tokenList);
    }

    #ifdef DEBUG
    printDebug("parsing primary unary");
    tabs++;
    #endif

    Expr* e1 = parsePrimary(tokenList);

    if (has_op) {
        Expr* unary = newUnaryExpr(e1, op);
        unary->unary.got_op = true;
        e1 = unary;
    }
    
    #ifdef DEBUG
    tabs--;
    #endif

    return e1;
}

Expr* parsePrimary(TOKEN_LIST* tokenList) {
    Expr *e1 = malloc(sizeof(Expr));
    if (e1 == NULL)
    {
        perror("Error factor malloc.\n");
        exit(EXIT_FAILURE);
    }

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == IDENTIFIER) {
        #ifdef DEBUG
        for (int i = 0; i < tabs; i++) {
            printf("   ");
        }
        printf("got identifier\n");
        #endif

        char* name = expectToken(tokenList, IDENTIFIER).value.value_str;
        if (showNextToken(tokenList).type == L_SQ_BRACKET) {
            expectToken(tokenList, L_SQ_BRACKET);

            #ifdef DEBUG
            printDebug("parsing expression for array access");
            tabs++;
            #endif

            e1->array.name = name;
            e1->type = EXPR_ARRAY;
            e1->array.index = parseExpression(tokenList);

            #ifdef DEBUG
            tabs--;
            #endif

            expectToken(tokenList, R_SQ_BRACKET);
        }
        else {
            e1->identifier.name = name;
            e1->type = EXPR_IDENTIFIER;
        }
    }
    /* Si le token est un literal */
    else if (nextToken.type == INT_NUMBER ||
        nextToken.type == FLOAT_NUMBER ||
        nextToken.type == STRING ||
        nextToken.type == KW_FALSE ||
        nextToken.type == KW_TRUE)
    {
        acceptToken(tokenList);
        e1->type = EXPR_LITERAL;
        if (nextToken.type == INT_NUMBER) {
            #ifdef DEBUG
            printDebug("got integer literal");
            #endif
            e1->literal.value_type = TK_INT;
            e1->literal.value.value_int = nextToken.value.value_int;
        }
        else if (nextToken.type == FLOAT_NUMBER) {
            #ifdef DEBUG
            printDebug("got float literal");
            #endif
            e1->literal.value_type = TK_FLOAT;
            e1->literal.value.value_float = nextToken.value.value_float;
        }
        else if (nextToken.type == KW_FALSE) {
            #ifdef DEBUG
            printDebug("got false boolean literal");
            #endif
            e1->literal.value_type = TK_BOOL;
            e1->literal.value.value_bool = false;
        }
        else if (nextToken.type == KW_TRUE) {
            #ifdef DEBUG
            printDebug("got true boolean literal");
            #endif
            e1->literal.value_type = TK_BOOL;
            e1->literal.value.value_bool = true;
        }
        else if (nextToken.type == STRING) {
            #ifdef DEBUG
            printDebug("got string literal");
            #endif
            e1->literal.value_type = TK_CHAR;
            e1->literal.value.value_str = nextToken.value.value_str;
        }
    }
    else if (nextToken.type == L_PAREN) {
        #ifdef DEBUG
        printDebug("parsing parenthesis");
        tabs++;
        #endif
        Expr* expr = parseParenth(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
        return expr;
    }
    else
    {
        if (nextToken.ownstr) {
            printf("Expecting identifier, literal or parenthesis at line : %d, column : %d, value : %s.\n", nextToken.line, nextToken.column, nextToken.value.value_str);
        }
        else {
            printf("Expecting identifier, literal or parenthesis at line : %d, column : %d, value : %s.\n", nextToken.line, nextToken.column, (char[]){nextToken.value.value_chr, '\0'});
        }
        exit(1);
    }
    
    return e1;
}

Expr* parseParenth(TOKEN_LIST* tokenList) {
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    Expr* e1 = parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, R_PAREN);

    return e1;
}

// ------------------------------------------- Statement parsing ----------------------------------------------
Stmt* stmt_create(stmt_t type, Decl *decl, Asmt* asmt, Expr *init_expr, Expr *expr, Expr *next_expr, Stmt *body, Stmt *else_body, Stmt *next) {
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = type;
    stmt->decl = decl;
    stmt->asmt = asmt;
    stmt->init_expr = init_expr;
    stmt->expr = expr;
    stmt->next_expr = next_expr;
    stmt->body = body;
    stmt->else_body = else_body;
    stmt->next = next;
    return stmt;
}

Stmt* parseStatement(TOKEN_LIST* tokenList) {
    Stmt* stmt;

    enum type_t nextTokenType = showNextToken(tokenList).type;
    if (nextTokenType == TK_INT ||
        nextTokenType == TK_FLOAT ||
        nextTokenType == TK_BOOL ||
        nextTokenType == TK_CHAR) {
        #ifdef DEBUG
        printDebug("parsing declaration");
        tabs++;
        #endif
        Decl* decl = parseDeclaration(tokenList);
        stmt = stmt_create(STMT_DECL, decl, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }
    else if (nextTokenType == KW_IF) {
        #ifdef DEBUG
        printDebug("parsing if statement");
        tabs++;
        #endif
        IfStmt* if_stmt = parseIfStatement(tokenList);
        stmt = stmt_create(STMT_IF_ELSE, NULL, NULL, NULL, if_stmt->condition, NULL, if_stmt->then_branch, if_stmt->else_branch, NULL);
    }
    else if (nextTokenType == IDENTIFIER)
    {
        #ifdef DEBUG
        printDebug("parsing assignment");
        tabs++;
        #endif
        Asmt* asmt = parseAssignment(tokenList);
        stmt = stmt_create(STMT_ASSIGN, NULL, asmt, NULL, NULL, NULL, NULL, NULL, NULL);
    }
    else if (nextTokenType == KW_WHILE)
    {
        #ifdef DEBUG
        printDebug("parsing while statement");
        tabs++;
        #endif
        WhileStmt* while_stmt = parseWhileStatement(tokenList);
        stmt = stmt_create(STMT_WHILE, NULL, NULL, NULL, while_stmt->condition, NULL, while_stmt->body, NULL, NULL);
    }
    else
    {
        printf("Expecting statement (identifier, if or while)\n");
        exit(1);
    }
    #ifdef DEBUG
    tabs--;
    #endif
    return stmt;
}

Decl* parseDeclaration(TOKEN_LIST* tokenList) {
    Decl *decl = malloc(sizeof(Decl));
    if (decl == NULL)
    {
        perror("Error declaration malloc.\n");
        exit(EXIT_FAILURE);
    }

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == TK_INT ||
        nextToken.type == TK_FLOAT ||
        nextToken.type == TK_BOOL ||
        nextToken.type == TK_CHAR)
    {
        acceptToken(tokenList);
        decl->type = nextToken.type;
        decl->id = expectToken(tokenList, IDENTIFIER).value.value_str;
        // TODO : ajouter array
        // if (nextToken.type == L_CURL_BRACKET)
        // {
        //     expectToken(tokenList, L_CURL_BRACKET);
        //     decl->length = expectToken(tokenList, INT_NUMBER).value.value_int;
        //     expectToken(tokenList, R_CURL_BRACKET);
        // }
        expectToken(tokenList, TERMINATOR);
    }
    else
    {
        printf("No type specified at line : %d, column : %d.\n", nextToken.line, nextToken.column);
        exit(1);
    }
    
    return decl;
}

IfStmt* parseIfStatement(TOKEN_LIST* tokenList) {
    IfStmt *if_stmt = malloc(sizeof(IfStmt));
    if (if_stmt == NULL)
    {
        perror("Error if statement malloc.\n");
        exit(EXIT_FAILURE);
    }
    if_stmt->contains_else_stmt = false;

    expectToken(tokenList, KW_IF);
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    if_stmt->condition = parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, R_PAREN);

    expectToken(tokenList, L_CURL_BRACKET);
    while (showNextToken(tokenList).type != R_CURL_BRACKET)
    {
        #ifdef DEBUG
        printDebug("parsing statement");
        tabs++;
        #endif
        if_stmt->then_branch = parseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);
    if (showNextToken(tokenList).type == KW_ELSE)
    {
        expectToken(tokenList, KW_ELSE);
        expectToken(tokenList, L_CURL_BRACKET);
        #ifdef DEBUG
        printDebug("parsing else statement");
        tabs++;
        #endif
        if_stmt->contains_else_stmt = true;
        if_stmt->else_branch = parseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
        expectToken(tokenList, R_CURL_BRACKET);
    }

    return if_stmt;
}

WhileStmt* parseWhileStatement(TOKEN_LIST* tokenList) {
    WhileStmt *while_stmt = malloc(sizeof(WhileStmt));
    if (while_stmt == NULL)
    {
        perror("Error while statement malloc.\n");
        exit(EXIT_FAILURE);
    }

    expectToken(tokenList, KW_WHILE);
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    while_stmt->condition = parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, R_PAREN);
    
    expectToken(tokenList, L_CURL_BRACKET);
    while (showNextToken(tokenList).type != R_CURL_BRACKET)
    {
        #ifdef DEBUG
        printDebug("parsing statement");
        tabs++;
        #endif
        while_stmt->body = parseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);

    return while_stmt;
}

Asmt* parseAssignment(TOKEN_LIST* tokenList) {
    Asmt *asmt = malloc(sizeof(Asmt));
    if (asmt == NULL)
    {
        perror("Error assignment malloc.\n");
        exit(EXIT_FAILURE);
    }

    asmt->contains_lexpr = false;

    asmt->id = expectToken(tokenList, IDENTIFIER).value.value_str;
    if (showNextToken(tokenList).type != ASSIGN)
    {
        asmt->contains_lexpr = true;
        expectToken(tokenList, L_SQ_BRACKET);
        asmt->lexpr = parseExpression(tokenList);
        expectToken(tokenList, R_SQ_BRACKET);
    }
    expectToken(tokenList, ASSIGN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    asmt->rexpr = parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, TERMINATOR);

    return asmt;
}