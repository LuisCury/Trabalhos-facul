#include <stdio.h>
#include <stdbool.h>

struct node {
    char info;
    struct node *nodeptr;
};

void cria (struct node **lista) {
    *lista = NULL;
}

bool vazia (struct node **lista) {
    if (lista == NULL) {
        return true;
    }
    else {
        return false;
    }
}

bool cheia (void) {
    return false;
}

bool esta_na_lista (struct node **lista, char x) {
    bool listado = false;
    if (!vazia(lista)) {
        struct node *paux = *lista;
        while (paux != NULL) {
            if (paux->info == x) {
                listado = true;
                break;
            }
            paux = paux->nodeptr;
        }
    }
    return listado;
}

void insere (struct node **lista, struct node *novo_node, char x, bool *ok) {
    if (cheia()) {
        *ok = false;
    }
    else if (*lista == NULL) { 
        *lista = novo_node;
        (*lista)->info = x;
        (*lista)->nodeptr = NULL;
        *ok = true;
    }
    else {
        struct node *anterior = NULL;
        struct node *proximo = *lista;

        while (proximo != NULL && proximo->info < x) { 
            anterior = proximo;
            proximo = proximo->nodeptr;
        }

        if (anterior == NULL) { 
            novo_node->nodeptr = *lista;
            *lista = novo_node;
        } else {
            anterior->nodeptr = novo_node;
            novo_node->nodeptr = proximo;
        }
        novo_node->info = x;
    }
}

bool retira(struct node **lista, char x) {
    if (vazia(lista)) {
        return false;
    }

    struct node *anterior = NULL;
    struct node *atual = *lista;

    while (atual != NULL && atual->info != x) {
        anterior = atual;
        atual = atual->nodeptr;
    }

    if (atual == NULL) {
        return false;
    }

    if (anterior == NULL) {
        *lista = atual->nodeptr;
    } else {
        anterior->nodeptr = atual->nodeptr;
    }

    return true; 
}

bool pega_primeiro(struct node **lista, char *primeiro) {
    if (vazia(lista)) {
        return false;
    }
    *primeiro = (*lista)->info;
    return true;
}

bool pega_proximo(struct node *atual, char *proximo) {
    if (atual == NULL || atual->nodeptr == NULL) {
        return false;
    }
    *proximo = atual->nodeptr->info;
    return true;
}

