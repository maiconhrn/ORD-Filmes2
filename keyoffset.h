//
// Created by Maicon on 27/04/2019.
//

#ifndef FILMES_KEYOFFSET_H
#define FILMES_KEYOFFSET_H

#include <stdio.h>

typedef struct {
    int key;
    int offset;
} Keyoffset;

void init_keyoffset(Keyoffset *keyOffset);

#endif //FILMES_KEYOFFSET_H
