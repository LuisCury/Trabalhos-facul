#include "tad.h"
#include <iostream>

using namespace std;

int main (void) {

    Node raiz(50);

    Node *ptr = &raiz;

    raiz.adiciona_node(&ptr, 25);
    raiz.adiciona_node(&ptr, 10);
    raiz.adiciona_node(&ptr, 40);
    raiz.adiciona_node(&ptr, 75);
    raiz.adiciona_node(&ptr, 60);
    raiz.adiciona_node(&ptr, 100);

    raiz.imprime_arvore(ptr);

    if (raiz.esta_na_arvore(ptr, 100)) {
        printf(" Valor 100 encontrado\n");
    }
    else {
        printf("Valor 100 nao encontrado\n");
    }


    printf("\n %d \n", raiz.niveis_arvore(ptr));

    raiz.adiciona_node(&ptr, 0);

    raiz.retira_node(&ptr, 100);

    raiz.imprime_arvore(ptr);

    printf("\n %d \n", raiz.niveis_arvore(ptr));

    if (raiz.esta_na_arvore(ptr, 100)) {
        printf(" Valor 100 encontrado\n");
    }
    else {
        printf("Valor 100 nao encontrado\n");
    }

    return 0;
}
