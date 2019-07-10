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
    KeyOffset keyoffsets[MAX_KEYS];
    int adjs[MAX_KEYS + 1];
} Page;

typedef struct {
    int qtd_keys;
    KeyOffset keyoffsets[MAX_KEYS + 1];
    int adjs[MAX_KEYS + 2];
} Page_Aux;

Bool create_btree(KeyOffset *keyoffset, int qtd);

Bool search_btree(int key);

#endif //TRABALHO2_BTREE_H
