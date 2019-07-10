//
// Created by Maicon on 02/07/2019.
//

#include <stdio.h>
#include <stdlib.h>

#include "btree.h"
#include "keyoffset.h"

enum {
    PROMOTION,
    ERROR,
    WITHOUT_PROMOTION
};

/* BEGIN AUX */

void read_page(int rrn, FILE *file, Page *page) {
    fseek(file, sizeof(int), SEEK_SET);
    fseek(file, rrn * sizeof(Page), SEEK_CUR);
    fread(page, sizeof(Page), 1, file);
}

void write_page(int rrn, Page page, FILE *file) {
    fseek(file, sizeof(int), SEEK_SET);
    fseek(file, rrn * sizeof(Page), SEEK_CUR);
    fwrite(&page, sizeof(Page), 1, file);
}

void init_page(Page *page) {
    page->qtd_keys = 0;
    for (int i = 0; i < MAX_KEYS; ++i) {
        init_keyoffset(&page->keyoffsets[i]);
        page->adjs[i] = -1;
    }
    page->adjs[MAX_KEYS] = -1;
}

Bool search_in_page(int key, Page page, int *pos) {
    int i = 0;
    while (i < page.qtd_keys && key > page.keyoffsets[i].key) {
        i++;
    }
    *pos = i;
    if (*pos < page.qtd_keys && key == page.keyoffsets[*pos].key) {
        return true;
    } else {
        return false;
    }
}

void insert_in_page(KeyOffset keyoffset, int adj_r, Page *page) {
    int i = page->qtd_keys;
    while (i > 0 && keyoffset.key < page->keyoffsets[i - 1].key) {
        page->keyoffsets[i] = page->keyoffsets[i - 1];
        page->adjs[i + 1] = page->adjs[i];
        i--;
    }
    page->qtd_keys++;
    page->keyoffsets[i] = keyoffset;
    page->adjs[i + 1] = adj_r;
}

int rrn_new_page(FILE *file) {
    int size_page = sizeof(Page);
    int size_header = sizeof(int);
    fseek(file, 0, SEEK_END);
    int offset = ftell(file);

    return (offset - size_header) / size_page;
}

void divide(KeyOffset keyoffset, int adj_r, Page *page, KeyOffset *keyoffset_pro, int *adj_r_pro, Page *new_page,
            FILE *file) {
    Page_Aux page_aux;
    int i = 0;


    //copy page to page_aux
    page_aux.qtd_keys = page->qtd_keys;
    for (i = 0; i < MAX_KEYS; ++i) {
        page_aux.keyoffsets[i] = page->keyoffsets[i];
        page_aux.adjs[i] = page->adjs[i];
    }
    page_aux.keyoffsets[MAX_KEYS].key = -1;
    page_aux.keyoffsets[MAX_KEYS].offset = -1;
    page_aux.adjs[MAX_KEYS] = page->adjs[MAX_KEYS];
    page_aux.adjs[MAX_KEYS + 1] = -1;


    i = MAX_KEYS;
    while ((page_aux.keyoffsets[i - 1].key == -1 || keyoffset.key < page_aux.keyoffsets[i - 1].key) && i > 0) {
        page_aux.keyoffsets[i] = page_aux.keyoffsets[i - 1];
        page_aux.adjs[i + 1] = page_aux.adjs[i];
        i--;
    }
    page_aux.keyoffsets[i] = keyoffset;
    page_aux.adjs[i + 1] = adj_r;

    int half = (MAX_KEYS + 1) / 2;
    *adj_r_pro = rrn_new_page(file);
    *keyoffset_pro = page_aux.keyoffsets[half];

    i = 0;
    init_page(page);
    while (i < half) {
        page->keyoffsets[i] = page_aux.keyoffsets[i];
        page->adjs[i] = page_aux.adjs[i];
        page->qtd_keys++;
        i++;
    }
    page->adjs[i] = page_aux.adjs[i];

    init_page(new_page);
    i = half + 1;
    while (i < MAX_KEYS + 1) {
        new_page->keyoffsets[new_page->qtd_keys] = page_aux.keyoffsets[i];
        new_page->adjs[new_page->qtd_keys] = page_aux.adjs[i];
        new_page->qtd_keys++;
        i++;
    }
    new_page->adjs[new_page->qtd_keys] = page_aux.adjs[i];
}

Bool have_space_to_insert(int key, Page page) {
    return page.qtd_keys < MAX_KEYS;
}

/* END AUX */

/* BEGIN OPERATIONS */

Bool search(int rrn, int key, int *rrn_found, int *pos_found, FILE *file) {
    Page page;
    init_page(&page);
    int pos;

    if (rrn == -1) {
        return false;
    } else {
        read_page(rrn, file, &page);
        if (search_in_page(key, page, &pos)) {
            *rrn_found = rrn;
            *pos_found = pos;
            return true;
        } else {
            return search(page.adjs[pos], key, rrn_found, pos_found, file);
        }
    }
}

int insert(int rrn_curr, KeyOffset keyoffset, int *adj_r_pro, KeyOffset *keyoffset_pro, FILE *file) {
    Page page, new_page;
    init_page(&page);
    init_page(&new_page);
    int pos, rrn_pro = *adj_r_pro;
    KeyOffset _keyoffset_pro = *keyoffset_pro;
    Bool is_found;

    if (rrn_curr == -1) {
        *keyoffset_pro = keyoffset;
        *adj_r_pro = -1;
        return PROMOTION;
    } else {
        read_page(rrn_curr, file, &page);
        is_found = search_in_page(keyoffset.key, page, &pos);
    }
    if (is_found) {
        printf("Chave ja existente no arquivo de registros.\n");
        return ERROR;
    }
    int _return = insert(page.adjs[pos], keyoffset, &rrn_pro, &_keyoffset_pro, file);
    if (_return == WITHOUT_PROMOTION || _return == ERROR) {
        return _return;
    } else {
        if (have_space_to_insert(_keyoffset_pro.key, page)) {
            insert_in_page(_keyoffset_pro, rrn_pro, &page);
            write_page(rrn_curr, page, file);
            return WITHOUT_PROMOTION;
        } else {
            divide(_keyoffset_pro, rrn_pro, &page, keyoffset_pro, adj_r_pro, &new_page, file);
            write_page(rrn_curr, page, file);
            write_page(*adj_r_pro, new_page, file);
            return PROMOTION;
        }
    }
}

/* END OPERATIONS */

Bool create_btree(KeyOffset *keyoffset, int qtd) {
    FILE *btree = fopen("btree.dat", "w+b");
    int root = 0;
    Page new_page;

    fwrite(&root, sizeof(int), 1, btree);
    init_page(&new_page);
    write_page(rrn_new_page(btree), new_page, btree);

    int adj_r_pro = -1;
    KeyOffset keyoffset_pro;
    int rrn_page;
    for (int i = 0; i < qtd; ++i) {
        printf("Insercao da chave %d\n", keyoffset[i].key);
        if (insert(root, keyoffset[i], &adj_r_pro, &keyoffset_pro, btree) == PROMOTION) {
            init_page(&new_page);
            new_page.keyoffsets[0] = keyoffset_pro;
            new_page.adjs[0] = root;
            new_page.adjs[1] = adj_r_pro;
            rrn_page = rrn_new_page(btree);
            write_page(rrn_page, new_page, btree);
            root = rrn_page;
        }
    }

    fseek(btree, 0, SEEK_SET);
    fwrite(&root, sizeof(root), 1, btree);
    fclose(btree);

    return true;
}

Bool search_btree(int key) {
    FILE *btree = fopen("btree.dat", "rb");
    int root = -1, rrn_found = -1, pos_found = -1;
    Page page;

    if (btree != NULL) {
        fclose(btree);
        btree = fopen("btree.dat", "r+b");
        fread(&root, sizeof(int), 1, btree);

        if (search(root, key, &rrn_found, &pos_found, btree)) {
            read_page(rrn_found, btree, &page);

            //TODO buscar o registro em daodos.dat e exibir

            return true;
        }

        printf("Registro com chave %d nao encontrado\n", key);
        return false;
    }

    return false;
}
