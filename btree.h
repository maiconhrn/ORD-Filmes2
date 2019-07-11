//
// Created by Maicon on 02/07/2019.
//

#ifndef TRABALHO2_BTREE_H
#define TRABALHO2_BTREE_H

#define MAX_KEYS 4

#include "bool.h"
#include "keyoffset.h"

typedef struct {
    int qtd_keys;
    Keyoffset keyoffsets[MAX_KEYS];
    int adjs[MAX_KEYS + 1];
} Page;

typedef struct {
    int qtd_keys;
    Keyoffset keyoffsets[MAX_KEYS + 1];
    int adjs[MAX_KEYS + 2];
} Page_Aux;

short read_rec(char *recbuff, FILE *fd);

Bool create_btree(Keyoffset *keyoffsets, int qtd);

Bool search_btree(int key);

Bool insert_btree(char *reg, short reg_size);

#endif //TRABALHO2_BTREE_H
