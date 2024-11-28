#include "tad.h"

void imprime_lista(struct node *lista) {
    struct node *atual = lista;
    while (atual != NULL) {
        printf("%c ", atual->info);
        atual = atual->nodeptr;
    }
    printf("\n");
}


int main (void) {

    struct node *lista;
    struct node primeiro, segundo, terceiro, quarto;
    bool deu_certo;

    cria(&lista);

    insere(&lista, &primeiro, 'A', &deu_certo);
    insere(&lista, &segundo, 'B', &deu_certo);
    insere(&lista, &terceiro, 'C', &deu_certo);
    insere(&lista, &quarto, 'D', &deu_certo);

    if (esta_na_lista(&lista, 'C')) {
        printf("Positivo, capitao\n");
    }
    else {
        printf("Negativo, capitao\n");
    }

    retira(&lista, 'B');

    imprime_lista(lista);

    return 0;
}
