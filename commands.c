//
// Created by Maicon on 02/07/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "btree.h"
#include "keyoffset.h"

short read_rec(char *recbuff, FILE *fd) {
    short rec_lgth;

    if (fread(&rec_lgth, sizeof(rec_lgth), 1, fd) == 0) {
        return 0;
    }

    rec_lgth = fread(recbuff, sizeof(char), rec_lgth, fd);
    recbuff[rec_lgth] = '\0';

    return rec_lgth;
}

Bool convert_to_btree(char *source_fname) {
    FILE *fsource = fopen(source_fname, "rb");
    int qtd_reg = 0, reg_offset = sizeof(int), reg_key;
    short reg_size = 0;
    char reg[500];

    if (fsource != NULL) {
        fread(&qtd_reg, sizeof(qtd_reg), 1, fsource);
        KeyOffset keyOffsetArr[qtd_reg];

        for (int i = 0; i < qtd_reg; ++i) {
            reg_size = read_rec(reg, fsource);
            reg_key = atoi(strtok(reg, "|"));
            keyOffsetArr[i].key = reg_key;
            keyOffsetArr[i].offset = reg_offset;
            reg_offset += reg_size + sizeof(reg_size);
        }

        create_btree(keyOffsetArr, qtd_reg);
        fclose(fsource);

        return true;
    }
    printf("Arquivo de dados não encontrado!\n");

    return false;
}

void perform_operation(char *fname) {
    FILE *operations = fopen(fname, "r");
    char line[500], *operation, *reg;
    int key;

    while (fgets(line, 499, operations) != NULL) {
        operation = strtok(line, " \n");
        printf("\n");
        switch (atoi(operation)) {
            case 1:
                key = atoi(strtok(NULL, " \n"));
                printf("Busca pelo registro de chave \"%d\"\n", key);
                search_btree(key);
                break;
            case 2:
                reg = strtok(NULL, "\n");

                break;
            default:
                break;
        }
    }

    fclose(operations);
}
