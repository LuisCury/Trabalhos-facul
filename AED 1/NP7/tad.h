#ifndef TAD_H
#define TAD_H

#include <string>
using namespace std;

class Node {
    public:
        Node(int valor);
        ~Node();

        void setValor (int valor);
        int getValor () const;

        void adiciona_node (Node **node_adicionado, int valor_adicionado);
        void imprime_arvore (Node *raiz) const;
        int niveis_arvore (Node *raiz) const;

    private:
        class Node *esq;
        class Node *dir;
        int valor;
};

#endif
