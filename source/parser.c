#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

/**
* @brief Retourne le k-ième token de la liste sans le consommer.
*/
TOKEN showNextToken(TOKEN_LIST* tokenList, int k) {
    return tokenList->tokens[k];
}

/**
* @brief Consomme le token courant en l'enlevant de la liste.
*/
void acceptToken(TOKEN_LIST* tokenList) {
    tokenList++;
}

/**
* @brief consomme le lexème courant s’il est du bon type (kind). Sinon
* retourne une erreur à l’utilisateur en indiquant la ligne et la colonne d’erreur.
*/
void expectToken(TOKEN_LIST* tokenList, enum TOKEN_TYPE tokenType) {
    TOKEN token = showNextToken(tokenList, 0);
    if (token.type == tokenType) {
        acceptToken(tokenList);
    }
    else {
        printf("Syntax error line %d, column %d. Expected %d, got %d", token.line, token.column, tokenType, token.type);
        exit(0); /* A enlever : correction d'erreurs multiples */
    }
}

/**
* @brief Analyse récursive descendante LL(1).
*/
void parser(TOKEN_LIST* tokenList) {
    return;
}