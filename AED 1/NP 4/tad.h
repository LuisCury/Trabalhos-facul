#include <stdio.h>
#include <stdbool.h>

struct node {
    char info;
    struct node *proximo;
};

void cria_fila (struct node*node,char adicionado, struct node **primeiro, struct node **ultimo) {
  
    // Associando o pontiero primeiro ao primeiro node criado   
    (*primeiro) = node;

    // Adicionando o char x ao campo info do node
    node->info = adicionado;

    // Ligando o campo ponteiro do node ao null
    node->proximo = NULL;

    // Ligando o ponterio ultimo ao ultimo node criado
    (*ultimo) = node;
}

void adiciona_node (struct node *node_antigo, struct node *node_novo, char adicionado, struct node **ultimo) {

    // Ligando o campo ponteiro do node antigo ao node novo
    node_antigo->proximo = node_novo;

    // Adicionando o char x ao campo info do node
    node_novo->info = adicionado;

    // Ligando o campo ponteiro do node ao null
    node_novo->proximo = NULL;

    // Ligando o ponterio ultimo ao ultimo node criado
    (*ultimo) = node_novo;
}

bool fila_vazia (struct node **primeiro) {

    // Verificacao se a fila está vazia atraves do ponteiro que indica o primeiro node
    if (primeiro == NULL) {
        return true;
    }
    else {
        return false;
    }
}

bool fila_cheia (void) {
    
    // Para este codigo a fila nunca estara cheia
    return false;
}

void retira_node (struct node **primeiro, char *char_retirado) {

    struct node *auxiliar = (*primeiro);

    // Trocando o lugar do ponteiro que sinaliza o primeiro node
    (*primeiro) = auxiliar->proximo;

    // Colocando a info do node na variavel retirado
    *char_retirado = auxiliar->info;

    // colocando o ponteiro do node no null
    auxiliar->proximo = NULL;

}
