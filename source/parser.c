#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

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
TOKEN expectToken(TOKEN_LIST* tokenList, enum TOKEN_TYPE tokenType) {
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
void parser(TOKEN_LIST* tokenList) {
    /* Initialise à 0 l'indice du token courant */
    tokenList->indexToken = 0;

    /* TODO : remove comments */
    Program *program = parseProgram(tokenList);
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

Program* parseProgram(TOKEN_LIST* tokenList) {
    Program *program = malloc(sizeof(Program));
    if (program == NULL)
    {
        perror("Error program malloc.\n");
        exit(EXIT_FAILURE);
    }
    program->nb_decl = 0;
    program->nb_stmt = 0;
    
    #ifdef DEBUG
    printDebug("Entering program");
    tabs++;
    #endif
    expectToken(tokenList, TYPE_INT);
    expectToken(tokenList, KW_MAIN);
    expectToken(tokenList, L_PAREN);
    expectToken(tokenList, R_PAREN);
    expectToken(tokenList, L_CURL_BRACKET);
    enum TOKEN_TYPE nextTokenType = showNextToken(tokenList).type;
    while (nextTokenType != R_CURL_BRACKET) {
        if (nextTokenType == TYPE_INT ||
            nextTokenType == TYPE_FLOAT ||
            nextTokenType == TYPE_BOOL ||
            nextTokenType == TYPE_CHAR) {
            #ifdef DEBUG
            printDebug("parsing declaration");
            tabs++;
            #endif
            program->decl = realloc(program->decl, sizeof(Decl*) * (program->nb_decl + 1));
            program->decl[program->nb_decl] = parseDeclaration(tokenList);
            program->nb_decl++;
        }
        else {
            #ifdef DEBUG
            printDebug("parsing statement");
            tabs++;
            #endif
            program->stmt = realloc(program->stmt, sizeof(Stmt*) * (program->nb_stmt + 1));
            program->stmt[program->nb_stmt] = parseStatement(tokenList);
            program->nb_stmt++;
        }
        nextTokenType = showNextToken(tokenList).type;
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);

    return program;
}

Decl* parseDeclaration(TOKEN_LIST* tokenList) {
    Decl *decl = malloc(sizeof(Decl));
    if (decl == NULL)
    {
        perror("Error declaration malloc.\n");
        exit(EXIT_FAILURE);
    }

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == TYPE_INT ||
        nextToken.type == TYPE_FLOAT ||
        nextToken.type == TYPE_BOOL ||
        nextToken.type == TYPE_CHAR)
    {
        acceptToken(tokenList);
        decl->type = nextToken.type;
        decl->id = expectToken(tokenList, IDENTIFIER).value.value_str;
        if (nextToken.type == L_CURL_BRACKET)
        {
            expectToken(tokenList, L_CURL_BRACKET);
            decl->is_array = true;
            decl->length = expectToken(tokenList, INT_NUMBER).value.value_int;
            expectToken(tokenList, R_CURL_BRACKET);
        }
        expectToken(tokenList, TERMINATOR);
    }
    else
    {
        printf("No type specified at line : %d, column : %d.\n", nextToken.line, nextToken.column);
        exit(1);
    }
    
    return decl;
}

Stmt* parseStatement(TOKEN_LIST* tokenList) {
    Stmt *stmt = malloc(sizeof(Stmt));
    if (stmt == NULL)
    {
        perror("Error statement malloc.\n");
        exit(EXIT_FAILURE);
    }

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == IDENTIFIER)
    {
        #ifdef DEBUG
        printDebug("parsing assignment");
        tabs++;
        #endif
        stmt->type = ASSIGNMENT;
        stmt->asmt = parseAssignment(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    else if (nextToken.type == KW_WHILE)
    {
        #ifdef DEBUG
        printDebug("parsing while statement");
        tabs++;
        #endif
        stmt->type = WHILE_STATEMENT;
        stmt->while_stmt = parseWhileStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    else if (nextToken.type == KW_IF)
    {
        #ifdef DEBUG
        printDebug("parsing if statement");
        tabs++;
        #endif
        stmt->type = IF_STATEMENT;
        stmt->if_stmt = parseIfStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    else
    {
        printf("Expecting statement (identifier, if or while)\n");
        exit(1);
    }

    return stmt;
}

Asmt* parseAssignment(TOKEN_LIST* tokenList) {
    Asmt *asmt = malloc(sizeof(Asmt));
    if (asmt == NULL)
    {
        perror("Error assignment malloc.\n");
        exit(EXIT_FAILURE);
    }

    asmt->id = expectToken(tokenList, IDENTIFIER).value.value_str;
    if (showNextToken(tokenList).type == L_SQ_BRACKET) {
        expectToken(tokenList, L_SQ_BRACKET);
        #ifdef DEBUG
        printDebug("parsing expression");
        tabs++;
        #endif
        asmt->contains_lexpr = true;
        asmt->lexpr = parseExpression(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
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

IfStmt* parseIfStatement(TOKEN_LIST* tokenList) {
    IfStmt *if_stmt = malloc(sizeof(IfStmt));
    if (if_stmt == NULL)
    {
        perror("Error if statement malloc.\n");
        exit(EXIT_FAILURE);
    }
    if_stmt->nb_stmt = 0;

    ElseStmt *else_stmt = malloc(sizeof(ElseStmt));
    if (else_stmt == NULL)
    {
        perror("Error else statement malloc.\n");
        exit(EXIT_FAILURE);
    }
    else_stmt->nb_stmt = 0;
    if_stmt->contains_elseStmt = false;

    expectToken(tokenList, KW_IF);
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    if_stmt->expr = parseExpression(tokenList);
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
        if_stmt->stmt = realloc(if_stmt->stmt, sizeof(Stmt*) * (if_stmt->nb_stmt + 1));
        if_stmt->stmt[if_stmt->nb_stmt] = parseStatement(tokenList);
        if_stmt->nb_stmt++;
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);
    if (showNextToken(tokenList).type == KW_ELSE)
    {
        #ifdef DEBUG
        printDebug("parsing else statement");
        tabs++;
        #endif
        if_stmt->contains_elseStmt = true;
        if_stmt->else_stmt = parseElseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return if_stmt;
}

ElseStmt* parseElseStatement(TOKEN_LIST* tokenList) {
    ElseStmt *else_stmt = malloc(sizeof(ElseStmt));
    if (else_stmt == NULL)
    {
        perror("Error else statement malloc.\n");
        exit(EXIT_FAILURE);
    }
    else_stmt->nb_stmt = 0;

    expectToken(tokenList, KW_ELSE);
    expectToken(tokenList, L_CURL_BRACKET);
    while (showNextToken(tokenList).type != R_CURL_BRACKET)
    {
        #ifdef DEBUG
        printDebug("parsing statement");
        tabs++;
        #endif
        else_stmt->stmt = realloc(else_stmt->stmt, sizeof(Stmt*) * (else_stmt->nb_stmt + 1));
        else_stmt->stmt[else_stmt->nb_stmt] = parseStatement(tokenList);
        else_stmt->nb_stmt++;
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);

    return else_stmt;
}

WhileStmt* parseWhileStatement(TOKEN_LIST* tokenList) {
    WhileStmt *while_stmt = malloc(sizeof(WhileStmt));
    if (while_stmt == NULL)
    {
        perror("Error while statement malloc.\n");
        exit(EXIT_FAILURE);
    }
    while_stmt->nb_stmt = 0;

    expectToken(tokenList, KW_WHILE);
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    while_stmt->expr = parseExpression(tokenList);
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
        while_stmt->stmt = realloc(while_stmt->stmt, sizeof(Stmt*) * (while_stmt->nb_stmt + 1));
        while_stmt->stmt[while_stmt->nb_stmt] = parseStatement(tokenList);
        while_stmt->nb_stmt++;
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);

    return while_stmt;
}

Expr* parseExpression(TOKEN_LIST* tokenList) {
    Expr *expr = malloc(sizeof(Expr));
    if (expr == NULL)
    {
        perror("Error expression malloc.\n");
        exit(EXIT_FAILURE);
    }
    expr->nb_rconj = 0;

    #ifdef DEBUG
    printDebug("parsing conjunction");
    tabs++;
    #endif
    expr->lconj = parseConjunction(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    while (showNextToken(tokenList).type == OR) {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing conjunction");
        tabs++;
        #endif
        expr->rconj = realloc(expr->rconj, sizeof(Conj*) * (expr->nb_rconj + 1));
        expr->rconj[expr->nb_rconj] = parseConjunction(tokenList);
        expr->nb_rconj++;
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return expr;
}

Conj* parseConjunction(TOKEN_LIST* tokenList) {
    Conj *conj = malloc(sizeof(Conj));
    if (conj == NULL)
    {
        perror("Error conjonction malloc.\n");
        exit(EXIT_FAILURE);
    }
    conj->nb_requal = 0;

    #ifdef DEBUG
    printDebug("parsing equality");
    tabs++;
    #endif
    conj->lequal = parseEquality(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    while (showNextToken(tokenList).type == AND) {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing equality");
        tabs++;
        #endif
        conj->requal = realloc(conj->requal, sizeof(Equal*) * (conj->nb_requal + 1));
        conj->requal[conj->nb_requal] = parseEquality(tokenList);
        conj->nb_requal++;
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return conj;
}

Equal* parseEquality(TOKEN_LIST* tokenList) {
    Equal *equal = malloc(sizeof(Equal));
    if (equal == NULL)
    {
        perror("Error expression malloc.\n");
        exit(EXIT_FAILURE);
    }
    
    #ifdef DEBUG
    printDebug("parsing relation");
    tabs++;
    #endif
    equal->lrelation = parseRelation(tokenList);
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
        equal->rrelation = parseRelation(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return equal;
}
    
Relation* parseRelation(TOKEN_LIST* tokenList) {
    Relation *relation = malloc(sizeof(Relation));
    if (relation == NULL)
    {
        perror("Error relation malloc.\n");
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
    printDebug("parsing addition");
    tabs++;
    #endif
    relation->laddition = parseAddition(tokenList);
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
        relation->raddition = parseAddition(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }

    return relation;
}

Addition* parseAddition(TOKEN_LIST* tokenList) {
    Addition *addition = malloc(sizeof(Addition));
    if (addition == NULL)
    {
        perror("Error addition malloc.\n");
        exit(EXIT_FAILURE);
    }
    addition->nb_term = 0;

    #ifdef DEBUG
    printDebug("parsing term");
    tabs++;
    #endif
    addition->term = parseTerm(tokenList);
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
        addition->rterm = realloc(addition->rterm, sizeof(Term*) * (addition->nb_term + 1));
        addition->rterm[addition->nb_term] = parseTerm(tokenList);
        addition->nb_term++;
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }
}

Term* parseTerm(TOKEN_LIST* tokenList) {
    Term *term = malloc(sizeof(Term));
    if (term == NULL)
    {
        perror("Error term malloc.\n");
        exit(EXIT_FAILURE);
    }
    term->nb_factor = 0;

    #ifdef DEBUG
    printDebug("parsing factor");
    tabs++;
    #endif
    term->factor = parseFactor(tokenList);
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
        term->rfactor = realloc(term->rfactor, sizeof(Factor*) * (term->nb_factor + 1));
        term->rfactor[term->nb_factor] = parseFactor(tokenList);
        term->nb_factor++;
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }

    return term;
}

Factor* parseFactor(TOKEN_LIST* tokenList) {
    Factor *factor = malloc(sizeof(Factor));
    if (factor == NULL)
    {
        perror("Error factor malloc.\n");
        exit(EXIT_FAILURE);
    }

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == SUBTRACTION ||
        nextToken.type == NOT)
    {
        acceptToken(tokenList);
        factor->unaryop = nextToken.type;
    }
    #ifdef DEBUG
    printDebug("parsing primary");
    tabs++;
    #endif
    factor->primary = parsePrimary(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif

    return factor;
}

Primary* parsePrimary(TOKEN_LIST* tokenList) {
    Primary *primary = malloc(sizeof(Primary));
    if (primary == NULL)
    {
        perror("Error primary malloc.\n");
        exit(EXIT_FAILURE);
    }

    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == IDENTIFIER) {
        primary->type = PRIMARY_IDENTIFIER;
        primary->identifier.id = expectToken(tokenList, IDENTIFIER).value.value_str;
        if (nextToken.type == L_SQ_BRACKET) {
            expectToken(tokenList, L_SQ_BRACKET);
            #ifdef DEBUG
            printDebug("parsing expression");
            tabs++;
            #endif
            primary->type = PRIMARY_ARRAY_ACCESS;
            primary->identifier.expr = parseExpression(tokenList);
            #ifdef DEBUG
            tabs--;
            #endif
            expectToken(tokenList, R_SQ_BRACKET);
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
        primary->type = PRIMARY_LITERAL;
        if (nextToken.type == INT_NUMBER) {
            primary->value_type = TYPE_INT;
            primary->value.value_int = nextToken.value.value_int;
        }
        else if (nextToken.type == FLOAT_NUMBER) {
            primary->value_type = TYPE_FLOAT;
            primary->value.value_float = nextToken.value.value_float;
        }
        else if (nextToken.type == KW_FALSE) {
            primary->value_type = TYPE_BOOL;
            primary->value.value_bool = false;
        }
        else if (nextToken.type == KW_TRUE) {
            primary->value_type = TYPE_BOOL;
            primary->value.value_bool = true;
        }
        else if (nextToken.type == STRING) {
            primary->value_type = TYPE_CHAR;
            primary->value.value_str = nextToken.value.value_str;
        }
    }
    else if (nextToken.type == L_PAREN) {
        #ifdef DEBUG
        printDebug("parsing parenthesis");
        tabs++;
        #endif
        primary->type = PRIMARY_PARENTHESIZED_EXPRESSION;
        primary->parent = parseParenth(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
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
    
    return primary;
}

Parenth* parseParenth(TOKEN_LIST* tokenList) {
    Parenth *parenth = malloc(sizeof(Parenth));
    if (parenth == NULL)
    {
        perror("Error parenthesis malloc.\n");
        exit(EXIT_FAILURE);
    }

    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    parenth->expr = parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, R_PAREN);

    return parenth;
}