#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <nb_keywords>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Le nombre de keywords doit être > 0\n");
        return 1;
    }

    const char* KEYWORDS[] = {"if","while","input","func","for","in","body","begin","end","entity"};
    
    int nb_keywords = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

    FILE *file = fopen("source_code/code_test.c", "w");
    if (!file) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%s\n", KEYWORDS[i % nb_keywords]);
    }
    
    fclose(file);
    return 0;
}