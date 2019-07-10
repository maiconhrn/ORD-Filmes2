//
// Created by Maicon on 02/07/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Numero incorreto de argumentos!\n");
        exit(1);
    }

    if (strcmp(argv[1], "-c") == 0) {
        printf("Modo de importacao e exportacao para arvore-b ativado.\n");
        printf("Arquivo: \"dados.dat\" para \"btree.dat\"\n");
        if (convert_to_btree("dados.dat")) {
            printf("Exportacao executada com sucesso!\n");
        }
    } else if (strcmp(argv[1], "-e") == 0) {
        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        perform_operation(argv[2]);
    } else {
        fprintf(stderr, "Opcao \"%s\" nao suportada!\n", argv[1]);
    }

    return 0;
}