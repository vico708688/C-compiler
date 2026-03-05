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
void acceptToken(TOKEN_LIST* tokenList) {
    tokenList->indexToken++;
}

/**
* @brief consomme le lexème courant s’il est du bon type (kind). Sinon
* retourne une erreur à l’utilisateur en indiquant la ligne et la colonne d’erreur.
*/
void expectToken(TOKEN_LIST* tokenList, enum TOKEN_TYPE tokenType) {
    TOKEN token = showNextToken(tokenList);
    if (token.type != tokenType) {
        printf("Syntax error line %d, column %d. Expected %d, got %d.\n", token.line, token.column, tokenType, token.type);
        exit(1); /* A enlever : correction d'erreurs multiples */
    }
    acceptToken(tokenList);
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

void parseProgram(TOKEN_LIST* tokenList) {
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
            parseDeclaration(tokenList);
        }
        else {
            #ifdef DEBUG
            printDebug("parsing statement");
            tabs++;
            #endif
            parseStatement(tokenList);
        }
        nextTokenType = showNextToken(tokenList).type;
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);
}

void parseDeclaration(TOKEN_LIST* tokenList) {
    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == TYPE_INT ||
        nextToken.type == TYPE_FLOAT ||
        nextToken.type == TYPE_BOOL ||
        nextToken.type == TYPE_CHAR)
    {
        acceptToken(tokenList);
        expectToken(tokenList, IDENTIFIER);
        if (nextToken.type == L_CURL_BRACKET)
        {
            expectToken(tokenList, L_CURL_BRACKET);
            expectToken(tokenList, INT_NUMBER);
            expectToken(tokenList, R_CURL_BRACKET);
        }
        expectToken(tokenList, TERMINATOR);
    }
    else
    {
        printf("No type specified at line : %d, column : %d.\n", nextToken.line, nextToken.column);
        exit(1);
    }
}

void parseStatement(TOKEN_LIST* tokenList) {
    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == IDENTIFIER)
    {
        #ifdef DEBUG
        printDebug("parsing assignment");
        tabs++;
        #endif
        parseAssignment(tokenList);
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
        parseWhileStatement(tokenList);
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
        parseIfStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    else
    {
        printf("Expecting statement (identifier, if or while)\n");
        exit(1);
    }
}

void parseAssignment(TOKEN_LIST* tokenList) {
    expectToken(tokenList, IDENTIFIER);
    if (showNextToken(tokenList).type == L_SQ_BRACKET) {
        expectToken(tokenList, L_SQ_BRACKET);
        #ifdef DEBUG
        printDebug("parsing expression");
        tabs++;
        #endif
        parseExpression(tokenList);
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
    parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, TERMINATOR);
}

void parseIfStatement(TOKEN_LIST* tokenList) {
    expectToken(tokenList, KW_IF);
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    parseExpression(tokenList);
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
        parseStatement(tokenList);
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
        parseElseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
}

void parseElseStatement(TOKEN_LIST* tokenList) {
    expectToken(tokenList, KW_ELSE);
    expectToken(tokenList, L_CURL_BRACKET);
    while (showNextToken(tokenList).type != R_CURL_BRACKET)
    {
        #ifdef DEBUG
        printDebug("parsing statement");
        tabs++;
        #endif
        parseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);
}

void parseWhileStatement(TOKEN_LIST* tokenList) {
    expectToken(tokenList, KW_WHILE);
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    parseExpression(tokenList);
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
        parseStatement(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
    expectToken(tokenList, R_CURL_BRACKET);
}

void parseExpression(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing conjunction");
    tabs++;
    #endif
    parseConjunction(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    while (showNextToken(tokenList).type == OR) {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing conjunction");
        tabs++;
        #endif
        parseConjunction(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
}

void parseConjunction(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing equality");
    tabs++;
    #endif
    parseEquality(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    while (showNextToken(tokenList).type == AND) {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing equality");
        tabs++;
        #endif
        parseEquality(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
    }
}

void parseEquality(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing relation");
    tabs++;
    #endif
    parseRelation(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    
    TOKEN nextToken = showNextToken(tokenList);
    while (nextToken.type == EQUALITY ||
        nextToken.type == INEQUALITY)
    {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing relation");
        tabs++;
        #endif
        parseRelation(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }
}
    
void parseRelation(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing addition");
    tabs++;
    #endif
    parseAddition(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    
    TOKEN nextToken = showNextToken(tokenList);
    while (nextToken.type == LT ||
        nextToken.type == LTE ||
        nextToken.type == GT ||
        nextToken.type == GTE)
    {
        acceptToken(tokenList);
        #ifdef DEBUG
        printDebug("parsing addition");
        tabs++;
        #endif
        parseAddition(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }
}

void parseAddition(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing term");
    tabs++;
    #endif
    parseTerm(tokenList);
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
        parseTerm(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }
}

void parseTerm(TOKEN_LIST* tokenList) {
    #ifdef DEBUG
    printDebug("parsing factor");
    tabs++;
    #endif
    parseFactor(tokenList);
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
        parseFactor(tokenList);
        #ifdef DEBUG
        tabs--;
        #endif
        nextToken = showNextToken(tokenList);
    }
}

void parseFactor(TOKEN_LIST* tokenList) {
    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == SUBTRACTION ||
        nextToken.type == NOT)
    {
        acceptToken(tokenList);
    }
    #ifdef DEBUG
    printDebug("parsing primary");
    tabs++;
    #endif
    parsePrimary(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
}

void parsePrimary(TOKEN_LIST* tokenList) {
    TOKEN nextToken = showNextToken(tokenList);
    if (nextToken.type == IDENTIFIER) {
        expectToken(tokenList, IDENTIFIER);
        if (nextToken.type == L_SQ_BRACKET) {
            expectToken(tokenList, L_SQ_BRACKET);
            #ifdef DEBUG
            printDebug("parsing expression");
            tabs++;
            #endif
            parseExpression(tokenList);
            #ifdef DEBUG
            tabs--;
            #endif
            expectToken(tokenList, R_SQ_BRACKET);
        }
    }
    else if (nextToken.type == INT_NUMBER ||
        nextToken.type == FLOAT_NUMBER ||
        nextToken.type == KW_FALSE ||
        nextToken.type == KW_TRUE)
    {
        acceptToken(tokenList);
    }
    else if (nextToken.type == L_PAREN) {
        #ifdef DEBUG
        printDebug("parsing parenthesis");
        tabs++;
        #endif
        parseParenth(tokenList);
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
    
}

void parseParenth(TOKEN_LIST* tokenList) {
    expectToken(tokenList, L_PAREN);
    #ifdef DEBUG
    printDebug("parsing expression");
    tabs++;
    #endif
    parseExpression(tokenList);
    #ifdef DEBUG
    tabs--;
    #endif
    expectToken(tokenList, R_PAREN);
}

/**
* @brief Analyse récursive descendante LL(1).
*/
void parser(TOKEN_LIST* tokenList) {
    /* Initialise à 0 l'indice du token courant */
    tokenList->indexToken = 0;

    /* TODO : remove comments */
    parseProgram(tokenList);
}