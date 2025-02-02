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
        void retira_node (Node **raiz, int valor_retirado);
        bool esta_na_arvore (Node *raiz, int valor_procurado) const;
        void imprime_arvore (Node *raiz) const;
        int niveis_arvore (Node *raiz) const;

    private:
        class Node *esq;
        class Node *dir;
        int valor;
};

#endif
