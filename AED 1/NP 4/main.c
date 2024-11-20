#include "tad.h"

void imprimir_fila (struct node **primeiro) {

    // Criando variaveis auxiliares
    struct node *ponteiro_auxiliar = (*primeiro);
    int contador = 1;

    while (ponteiro_auxiliar != NULL) {
        
        // Imprimindo o node e char
        printf("NODE %d: %c", contador, ponteiro_auxiliar->info);
        printf("\n");
        contador = contador + 1;

        // Passando o ponteiro auxiliar para o proximo node
        ponteiro_auxiliar = ponteiro_auxiliar->proximo;

    }
}

int main (void) {

    // Criacao das variaveis
    struct node *ponterio_do_primeiro = NULL;
    struct node *ultimo = NULL;
    struct node primeiro, segundo, terceiro, quarto, quinto;
    char retirado;

    // Criando e adicionando node a fila
    cria_fila (&primeiro, 'A', &ponterio_do_primeiro, &ultimo);
    adiciona_node (&primeiro, &segundo, 'B', &ultimo);
    adiciona_node (&segundo, &terceiro, 'C', &ultimo);
    adiciona_node (&terceiro, &quarto, 'D', &ultimo);
    adiciona_node (&quarto, &quinto, 'E', &ultimo);

    // Imprimindo a fila atual
    imprimir_fila(&ponterio_do_primeiro);
    printf("\n");

    // Retirando um node
    retira_node(&ponterio_do_primeiro, &retirado);

    // Imprimindo a fila atual
    imprimir_fila(&ponterio_do_primeiro);
    printf("%c", retirado);
    printf("\n");

    // Verificando se a lista esta vazia ou cheia
    if (fila_vazia(&ponterio_do_primeiro)) {
        printf("A fila esta vazia\n");
    }
    else if (fila_cheia()) {
        printf("A fila esta vazia\n");
    }
    else {
        printf("A fila possui nodes e espacos para novos nodes\n");
    }

    return 0;
}
