#include "compiler.h"

#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "visitor.h"

void compile(char *text) {
    char* backup_text = text; /* Modification de pointeur text dans lexer(), il faut sauvegarder le pointeur d'origine */

	/* Analyse du code */
	#ifdef DEBUG
    printf("Analyse lexicale :\n-------------------------------------------------------------------------------\n");
	#endif

	TOKEN_LIST tokenList = lexer(&text);
	
	#ifdef DEBUG
    printf("Analyse syntaxique :\n-----------------------------------------------------------------------------\n");
	#endif
	
	Program *program = parser(&tokenList);

	Visitor printer = createPrettyPrinter();
    printer.visitProgram(&printer, program);

	freeTokens(&tokenList);
	free(backup_text);
}