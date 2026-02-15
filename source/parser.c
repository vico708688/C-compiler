#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

/**
* @brief Retourne le k-ième token de la liste sans l'enlever de la liste.
*/
TOKEN show_next(TOKEN_LIST* tokenList, int k) {
    return tokenList->tokens[0];
}

/**
* @brief Consomme le token courant en l'enlevant de la liste.
*/
void accept(TOKEN_LIST* tokenList) {
    return;
}

/**
* @brief consomme le lexème courant s’il est du bon type (kind). Sinon
* retourne une erreur à l’utilisateur en indiquant la ligne et la colonne d’erreur.
*/
void expect(TOKEN_LIST* tokenList, enum TOKEN_TYPE tokenType) {
    return;
}

/**
* @brief Analyse récursive descendente LL(2).
*/
void parser(TOKEN_LIST* tokenList) {
    return;
}