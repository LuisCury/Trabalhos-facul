#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

void must_init(bool test, const char* description)
{
    if (test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

// Estrutura de uma fila
typedef struct fila {
    char* itens;
    int frente, tras, tamanho;
} Fila;

// Struct para armazenar uma palavra
typedef struct palavra {
    Fila letras;
} Palavra;

// Inicializa uma fila com um tamanho variável
void inicializar_fila(Fila* f, int tamanho) {
    if (tamanho <=0) {
        printf("Erro: tamanho invalido para inicializar a fila.\n");
        exit(1);
    }
    f->itens = (char*)malloc(tamanho * sizeof(char));
    if (f->itens == NULL) {
        printf("Erro ao alocar memoria para a fila.\n");
        exit(1);
    }
    f->frente = 0;
    f->tras = -1;
    f->tamanho = tamanho;
}

// Enfileira um caractere
void enfileirar(Fila* f, char item) {
    if (f->tras < f->tamanho - 1) {
        f->tras++;
        f->itens[f->tras] = item;
    }
    else {
        printf("Erro: tentativa de enfileirar em uma fila cheia.\n");
        exit(1);
    }
}

char desenfileirar(Fila* f) {
    if (f->frente <= f->tras) {
        return f->itens[f->frente++];
    }
    return '\0'; // Retorna nulo se a fila estiver vazia
}

// Libera a memória alocada pela fila
void destruir_fila(Fila* f) {
    if (f->itens != NULL) {
        free(f->itens);
        f->itens = NULL;
    }
}

// Verifica e colore as letras da tentativa
void verificar_e_colorir(Palavra* resposta, Fila* tentativa, int cor_bola[8]) {
    int tamanho = resposta->letras.tamanho;
    Fila copia_resposta, copia_tentativa;
    inicializar_fila(&copia_resposta, tamanho);
    inicializar_fila(&copia_tentativa, tamanho);

    for (int i = 0; i < tamanho; i++) {
        enfileirar(&copia_resposta, resposta->letras.itens[i]);
        enfileirar(&copia_tentativa, tentativa->itens[i]);
    }

    char resultado[9];
    for (int i = 0; i < tamanho; i++) {
        resultado[i] = 'r';
    }

    for (int i = 0; i < tamanho; i++) {
        if (copia_tentativa.itens[i] == copia_resposta.itens[i]) {
            resultado[i] = 'v';
            copia_resposta.itens[i] = '*';
        }
    }

    for (int i = 0; i < tamanho; i++) {
        if (resultado[i] != 'v') {
            for (int j = 0; j < tamanho; j++) {
                if (copia_tentativa.itens[i] == copia_resposta.itens[j] && copia_resposta.itens[j] != '*') {
                    resultado[i] = 'a';
                    copia_resposta.itens[j] = '*';
                    break;
                }
            }
        }
    }

    for (int i = 0; i < tamanho; i++) {
        if (resultado[i] == 'v') {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);   //verde
            cor_bola[i] = 3;
        }
        else if (resultado[i] == 'a') {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);    //amarelo
            cor_bola[i] = 2;
        }
        else {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);   //vermelho
            cor_bola[i] = 1;
        }

    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    printf("\n");
    destruir_fila(&copia_resposta);
    destruir_fila(&copia_tentativa);
}

void remover_ultimo_elemento(Fila* f) {
    f->tras--;
}

// Verifica se o jogador acertou
bool verificar_se_ganhou(Palavra* resposta, Fila* tentativa) {
    for (int i = 0; i < resposta->letras.tamanho; i++) {
        if (resposta->letras.itens[i] != tentativa->itens[i]) {
            return false;
        }
    }
    return true;
}

void associando_cor_letra_a_bola(char letra, int cor, int alfabeto_bolas[25]) {
    int i = letra - 65;
    if (alfabeto_bolas[i] == 0) {
        alfabeto_bolas[i] = cor;
    }
    else if (alfabeto_bolas[i] == 1 && cor > 1) {
        alfabeto_bolas[i] = cor;
    }
    else  if (alfabeto_bolas[i] == 2 && cor > 2) {
        alfabeto_bolas[i] = cor;
    }
}

void escolher_palavra_aleatoria(Palavra* resposta, int numero_de_letras) {
    char linha[100];
    char palavras[100][9]; // Suporte até 8 letras
    int contador = 0;
    FILE* arquivo;
    errno_t err;
    if (numero_de_letras == 5) {
        err = fopen_s(&arquivo, "letras_5.txt", "r");
    }
    else if (numero_de_letras == 6) {
        err = fopen_s(&arquivo, "letras_6.txt", "r");
    }
    else if (numero_de_letras == 7) {
        err = fopen_s(&arquivo, "letras_7.txt", "r");
    }
    else {
        err = fopen_s(&arquivo, "letras_8.txt", "r");
    }


    if (err != 0 || arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo letras.txt.\n");
        exit(1);
    }

    while (fgets(linha, sizeof linha, arquivo)) {
        // Remove o caractere de nova linha, se presente
        linha[strcspn(linha, "\n")] = '\0';
        int tamanho = strlen(linha);
        if (tamanho >= 5 && tamanho <= 8) {
            strcpy_s(palavras[contador], sizeof(palavras[contador]), linha);
            contador++;
        }
    }
    fclose(arquivo);

    if (contador == 0) {
        printf("Erro: nenhuma palavra valida encontrada no arquivo.\n");
        exit(1);
    }

    srand((unsigned int)time(NULL));
    char* palavra_certa = palavras[rand() % contador];
    int tamanho = strlen(palavra_certa);

    inicializar_fila(&resposta->letras, tamanho);
    for (int i = 0; i < tamanho; i++) {
        enfileirar(&resposta->letras, palavra_certa[i]);
    }
}
