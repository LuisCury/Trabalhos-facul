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

bool Node::Node :: esta_na_arvore (Node *raiz, int valor_procurado) const {
    if (raiz == NULL) {
        return false;
    }
    else {
        if (raiz->getValor() == valor_procurado) {
            return true;
        }
        else {
            if (valor_procurado < raiz->getValor()) {
                return esta_na_arvore(raiz->esq, valor_procurado);
            }
            else {
                return esta_na_arvore(raiz->dir, valor_procurado);
            }
        }
    }
}


void Node::retira_node(Node **raiz, int valor_removido) {
    if (*raiz == NULL) {
        printf("Valor nao encontrado na arvore\n");
        return;
    }

    if (valor_removido < (*raiz)->getValor()) {
        retira_node(&(*raiz)->esq, valor_removido);
    } else if (valor_removido > (*raiz)->getValor()) {
        retira_node(&(*raiz)->dir, valor_removido);
    } else {
        Node *temp = *raiz;
        if ((*raiz)->esq == NULL) {
            *raiz = (*raiz)->dir;
        } else if ((*raiz)->dir == NULL) {
            *raiz = (*raiz)->esq;
        } else {
            Node *successorParent = *raiz;
            Node *successor = (*raiz)->dir;
            while (successor->esq != NULL) {
                successorParent = successor;
                successor = successor->esq;
            }
            if (successorParent != *raiz) {
                successorParent->esq = successor->dir;
            } else {
                successorParent->dir = successor->dir;
            }
            (*raiz)->valor = successor->valor;
            temp = successor;
        }
        delete temp;
    }
}



