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

    printf("\n %d \n", raiz.niveis_arvore(ptr));

    raiz.adiciona_node(&ptr, 0);

    printf("\n %d \n", raiz.niveis_arvore(ptr));

    return 0;
}
