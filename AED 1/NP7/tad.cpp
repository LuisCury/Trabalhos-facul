#include "tad.h" 
#include <iostream>
#include <string>

Node::Node (int valor) {
    this->valor = valor;
    this->esq = NULL;
    this->dir = NULL;
}

Node::~Node () {
    
}

void Node::setValor (int valor) {
    this->valor = valor;
}

int Node::getValor () const {
    return this->valor;
}

void Node::adiciona_node (Node **raiz, int valor_adicionado) {
    if ((*raiz) == NULL) {
        (*raiz) = new Node(valor_adicionado);
    }
    else {
        if (valor_adicionado < (*raiz)->getValor()) {
            adiciona_node(&(*raiz)->esq, valor_adicionado);
        }
        else if (valor_adicionado > (*raiz)->getValor()) {
            adiciona_node(&(*raiz)->dir, valor_adicionado);
        }
        else {
            printf("Valor ja existe na arvore\n");
        }
    }
}

void Node::imprime_arvore (Node *raiz) const {
    if (raiz != NULL) {
        imprime_arvore(raiz->esq);
        printf("%d ", raiz->getValor());
        imprime_arvore(raiz->dir);
    }
}

int Node::niveis_arvore(Node *raiz) const {
    if (raiz == NULL) {
        return 0;
    }
    else {
        int cont_esq = 0;
        int cont_dir = 0;

        if (raiz->esq != NULL) {
            cont_esq = niveis_arvore(raiz->esq);
        }

        if (raiz->dir != NULL) {
            cont_dir = niveis_arvore(raiz->dir);
        }
        
        if (cont_esq > cont_dir) {
            return cont_esq + 1;
        }
        else {
            return cont_dir + 1;
        }
        
    }
    
}


