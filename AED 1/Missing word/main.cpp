#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "funcoes_auxiliares.h"

#define TAM_X_TELA 1280
#define TAM_Y_TELA 960

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse"); 

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY* disp = al_create_display(TAM_X_TELA, TAM_Y_TELA);
    must_init(disp, "display");


    must_init(al_init_font_addon(), "font addon");
    must_init(al_init_ttf_addon(), "TTF");

    ALLEGRO_FONT* font = al_create_builtin_font();

    ALLEGRO_FONT* arial_titulo = al_load_ttf_font("arial.ttf", 50, 0);
    ALLEGRO_FONT* arial_letras = al_load_ttf_font("arial.ttf", 40, 0);

    must_init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    int x_mouse = 0;
    int y_mouse = 0;

    al_start_timer(timer);

    int numero_letras_palavra_certa = 5; //Numero de letras da palavra certa

    Palavra resposta;
    Fila tentativa;

    inicializar_fila(&resposta.letras, numero_letras_palavra_certa);
    inicializar_fila(&tentativa, 8);

    escolher_palavra_aleatoria(&resposta, numero_letras_palavra_certa);
 
    int contador_palavra = 1;   //Numero da palavra que está sendo digitada
    int contador_letra[7] = { 0 }; //Contador para indicar quantas letras tem na palavra de numero "n"

    char letras_das_palavras[7][8] = { "" };    //Matriz representado o tabuleiro de palavras(6 palavras com 8 letras cada)
    int cor_letras[8] = {0};    //Vetor representando as cores das letras
    int cor_bolinhas[6][8] = { 0 };
    int alfabeto_bolas[26] = { 0 };  //Vetor para guardar as cores das bolinhas do teclado

    bool ganhou = false;
    bool perdeu = false;
    bool jogar_novamente = false;
    bool mudou_dificuldade = false;

    bool pag_inicial = true;
    bool pag_jogo = false;
    bool regras_aberto = false;
    bool pag_dificuldade = false;

    while (1) {
        al_wait_for_event(queue, &event);

        switch (event.type) {
        case ALLEGRO_EVENT_TIMER:
            
            if (pag_jogo) {
                //Indicador da derrota do jogador
                if (contador_palavra == 7 && !ganhou) {
                    perdeu = true;
                }

                //Resetando todas as variaveis para comecar o jogo novamente
                if (jogar_novamente) {

                    //Zerando as cores das bolinhas
                    for (int i = 0; i < 6; i++) {
                        for (int j = 0; j < 8; j++) {
                            cor_bolinhas[i][j] = 0;
                            cor_letras[j] = 0;
                        }
                    }

                    //Voltando o contador de palavras ao começo
                    contador_palavra = 1;

                    //Zerando os contadores de letras
                    for (int i = 0; i < 7; i++) {
                        contador_letra[i] = 0;
                    }

                    //Zerando as cores das bolinhas das tentativas
                    for (int i = 0; i < 7; i++) {
                        for (int j = 0; j < 8; j++) {
                            letras_das_palavras[i][j] = '\0';
                        }
                    }

                    //Zerando as cores da bolinhas do teclado
                    for (int i = 0; i < 26; i++) {
                        alfabeto_bolas[i] = 0;
                    }

                    inicializar_fila(&tentativa, numero_letras_palavra_certa);

                    //Escolhendo nova palavra
                    escolher_palavra_aleatoria(&resposta, numero_letras_palavra_certa);

                    ganhou = false;
                    perdeu = false;
                    jogar_novamente = false;
                }
            }

            if (pag_inicial) {
                if (mudou_dificuldade) {
                    inicializar_fila(&resposta.letras, numero_letras_palavra_certa);

                    escolher_palavra_aleatoria(&resposta, numero_letras_palavra_certa);

                    mudou_dificuldade = false;
                }
            }

            redraw = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:

            if (pag_jogo) {

                //Registrando as letras apertadas
                if (event.keyboard.keycode >= ALLEGRO_KEY_A && event.keyboard.keycode <= ALLEGRO_KEY_Z) {
                    int i = contador_letra[contador_palavra];
                    if (i >= 0 && i < numero_letras_palavra_certa) { // Garante que não ultrapassa o tamanho da matriz
                        letras_das_palavras[contador_palavra][contador_letra[contador_palavra]] = 'A' + (event.keyboard.keycode - ALLEGRO_KEY_A); // Converte código da tecla para caractere
                        enfileirar(&tentativa, letras_das_palavras[contador_palavra][contador_letra[contador_palavra]]);
                        contador_letra[contador_palavra] = i + 1;
                    }
                }

                //Registrando o enter para mudar de linha somente quando as 5 letras estvirem postas
                if (contador_letra[contador_palavra] == numero_letras_palavra_certa && event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    verificar_e_colorir(&resposta, &tentativa, cor_letras);
                    for (int k = numero_letras_palavra_certa; k >= 0; k--) {
                        associando_cor_letra_a_bola(letras_das_palavras[contador_palavra][k], cor_letras[k], alfabeto_bolas);
                    }
                    contador_palavra = contador_palavra + 1;
                    ganhou = verificar_se_ganhou(&resposta, &tentativa);
                    inicializar_fila(&tentativa, numero_letras_palavra_certa);
                }

                //Registrando o backspace para apagar um letra
                if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                    if (contador_letra[contador_palavra] > 0) {
                        contador_letra[contador_palavra] = contador_letra[contador_palavra] - 1;
                        letras_das_palavras[contador_palavra][contador_letra[contador_palavra]] = ' ';
                        remover_ultimo_elemento(&tentativa);
                    }
                }
            }
 
        break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            x_mouse = event.mouse.x;
            y_mouse = event.mouse.y;

            //Eventos dos cliques na pagina inicial
            if (pag_inicial) {

                //Pagina de regras
                if (!pag_dificuldade && (x_mouse >= (TAM_X_TELA / 3) && x_mouse <= (2 * TAM_X_TELA / 3) && y_mouse >= 450 && y_mouse <= 550)) {
                    regras_aberto = true;
                }
                else if (regras_aberto && ((x_mouse >= (0.2 * TAM_X_TELA / 3) && x_mouse <= (2.8 * TAM_X_TELA / 3) && y_mouse >= (TAM_Y_TELA / 5) && y_mouse <= (4.5 * TAM_Y_TELA / 5)))) {
                    regras_aberto = true;
                }
                else {
                    regras_aberto = false;
                }

                //Indo para a tela de jogo
                if (!regras_aberto && !pag_dificuldade) {
                    if (x_mouse >= (1280 / 3) && x_mouse <= (2 * 1280 / 3) && y_mouse >= 300 && y_mouse <= 400) {
                        pag_jogo = true;
                        pag_inicial = false;
                    }
                }

                //Pgina da dificuldade
                if (!regras_aberto && !pag_dificuldade && (x_mouse >= (TAM_X_TELA / 3) && x_mouse <= (2 * TAM_X_TELA / 3) && y_mouse >= 600 && y_mouse <= 700)) {
                    pag_dificuldade = true;
                } 
                else if (pag_dificuldade && (x_mouse >= (0.2 * TAM_X_TELA / 3) && x_mouse <= (2.8 * TAM_X_TELA / 3) && y_mouse >= (TAM_Y_TELA / 5) && y_mouse <= (4.5 * TAM_Y_TELA / 5))) {

                    if (x_mouse >= 215 && x_mouse <= 505 && y_mouse >= 380 && y_mouse <= 560) {
                        numero_letras_palavra_certa = 5;
                        mudou_dificuldade = true;
                        pag_dificuldade = false;
                    }

                    if (x_mouse >= 755 && x_mouse <= 1045 && y_mouse >= 380 && y_mouse <= 560) {
                        numero_letras_palavra_certa = 6;
                        mudou_dificuldade = true;
                        pag_dificuldade = false;
                    }

                    if (x_mouse >= 215 && x_mouse <= 505 && y_mouse >= 600 && y_mouse <= 780) {
                        numero_letras_palavra_certa = 7;
                        mudou_dificuldade = true;
                        pag_dificuldade = false;
                    }

                    if (x_mouse >= 755 && x_mouse <= 1045 && y_mouse >= 600 && y_mouse <= 780) {
                        numero_letras_palavra_certa = 8;
                        mudou_dificuldade = true;
                        pag_dificuldade = false;
                    }

                }
                else {
                    pag_dificuldade = false;
                }

            }

            //Evento dos cliques na pagina de jogo
            if (pag_jogo) {

                //Botao de voltar
                if (!ganhou && !perdeu) {
                    if (x_mouse >= 50 && x_mouse <= 120 && y_mouse >= 50 && y_mouse <= 120) {
                        pag_inicial = true;
                        pag_jogo = false;
                        jogar_novamente = true;
                    }
                }

                //Botoes das telas de ganhou ou perdeu
                if (ganhou || perdeu) {
                    //Botao de jogar novamente
                    if (x_mouse >= 405 && x_mouse <= 630 && y_mouse >= 300 && y_mouse <= 500) {
                        jogar_novamente = true;
                    }

                    //Botao de pagina inicial
                    if (x_mouse >= 650 && x_mouse <= 875 && y_mouse >= 300 && y_mouse <= 500) {
                        pag_jogo = false;
                        pag_inicial = true;
                        jogar_novamente = true;
                        ganhou = false;
                        perdeu = false;
                    }
                }

                //Botao de enter
                if (contador_letra[contador_palavra] == numero_letras_palavra_certa && (x_mouse >= 860 && x_mouse <= 1065 && y_mouse >= 835 && y_mouse <= 905)) {
                    verificar_e_colorir(&resposta, &tentativa, cor_letras);
                    for (int k = numero_letras_palavra_certa; k >= 0; k--) {
                        associando_cor_letra_a_bola(letras_das_palavras[contador_palavra][k], cor_letras[k], alfabeto_bolas);
                    }
                    contador_palavra = contador_palavra + 1;
                    ganhou = verificar_se_ganhou(&resposta, &tentativa);
                    inicializar_fila(&tentativa, numero_letras_palavra_certa);
                }

                //Botao de delete
                if (x_mouse >= 995 && x_mouse <= 1140 && y_mouse >= 750 && y_mouse <= 820) {
                    if (contador_letra[contador_palavra] > 0) {
                        contador_letra[contador_palavra] = contador_letra[contador_palavra] - 1;
                        letras_das_palavras[contador_palavra][contador_letra[contador_palavra]] = ' ';
                        remover_ultimo_elemento(&tentativa);
                    }
                }

            }

        break;
        
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
        break;

        }

        if (done)
            break;

        if (redraw && al_is_event_queue_empty(queue)) {
            
            //Tela inicial
            if (pag_inicial) {
                al_clear_to_color(al_map_rgb(0, 0, 115));
                al_draw_text(arial_titulo, al_map_rgb(0, 0, 0), TAM_X_TELA / 2, 100, ALLEGRO_ALIGN_CENTRE, "MISSING WORD");

                //Retangulo do "jogar"
                al_draw_filled_rounded_rectangle((1280 / 3), 300, (2 * 1280 / 3), 400, 15, 15, al_map_rgba_f(0, 0, 0.9, 0.9));
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 330, ALLEGRO_ALIGN_CENTRE, "Jogar");

                //Retangulo do "regras"
                al_draw_filled_rounded_rectangle((TAM_X_TELA / 3), 450, (2 * TAM_X_TELA / 3), 550, 15, 15, al_map_rgba_f(0, 0, 0.9, 0.9));
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 480, ALLEGRO_ALIGN_CENTRE, "Regras");

                //Retangulo do "dificuldade"
                al_draw_filled_rounded_rectangle((TAM_X_TELA / 3), 600, (2 * TAM_X_TELA / 3), 700, 15, 15, al_map_rgba_f(0, 0, 0.9, 0.9));
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 630, ALLEGRO_ALIGN_CENTRE, "Dificuldade");

                if (regras_aberto) {
                    al_draw_filled_rounded_rectangle((0.2 * TAM_X_TELA / 3), TAM_Y_TELA / 5, (2.8 * TAM_X_TELA / 3), (4.5 * TAM_Y_TELA / 5), 25, 25, al_map_rgba_f(0, 0, 0.9, 1));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 256, ALLEGRO_ALIGN_CENTRE, "Missing word e um jogo no qual o jogador deve tentar acertar ");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 306, ALLEGRO_ALIGN_CENTRE, "uma palavra escolhida pelo computador aleatoriamente.");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 376, ALLEGRO_ALIGN_CENTRE, "Ele possui 6 chances para acertar a palavra, que pode");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 416, ALLEGRO_ALIGN_CENTRE, "conter de 5 a 8 letras, dependendo da dificuldade escolhida.");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 466, ALLEGRO_ALIGN_CENTRE, "Cada palavra chutada tera suas letras pintadas de acordo");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 506, ALLEGRO_ALIGN_CENTRE, "com sua existencia na palavra.");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 576, ALLEGRO_ALIGN_CENTRE, "Ou seja, caso fique a letra fique vermelha ela NAO esta");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 616, ALLEGRO_ALIGN_CENTRE, "contida na palavra correta; caso fique amarela ela");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 656, ALLEGRO_ALIGN_CENTRE, "esta contidana palavra, porem NAO esta na posicao correta;");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 696, ALLEGRO_ALIGN_CENTRE, "caso fique verde ela esta contida na palavra");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 736, ALLEGRO_ALIGN_CENTRE, "e esta na posicao correta.");
                }

                if (pag_dificuldade) {
                    al_draw_filled_rounded_rectangle((0.2 * TAM_X_TELA / 3), TAM_Y_TELA / 5, (2.8 * TAM_X_TELA / 3), (4.5 * TAM_Y_TELA / 5), 25, 25, al_map_rgba_f(0, 0, 0.9, 1));

                    //Texto da dificuldade
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 270, ALLEGRO_ALIGN_CENTRE, "Qual a dificuldade desejada?");

                    //Botao de facil
                    al_draw_filled_rounded_rectangle(215, 380, 505, 560, 15, 15, al_map_rgb(0, 0, 115));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 360, 420, ALLEGRO_ALIGN_CENTRE, "Facil");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 360, 460, ALLEGRO_ALIGN_CENTRE, "(5 letras)");

                    //Botao de medio
                    al_draw_filled_rounded_rectangle(755, 380, 1045, 560, 15, 15, al_map_rgb(0, 0, 115));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 900, 420, ALLEGRO_ALIGN_CENTRE, "Medio");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 900, 460, ALLEGRO_ALIGN_CENTRE, "(6 letras)");

                    //Botao de dificil
                    al_draw_filled_rounded_rectangle(215, 600, 505, 780, 15, 15, al_map_rgb(0, 0, 115));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 360, 640, ALLEGRO_ALIGN_CENTRE, "Dificil");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 360, 680, ALLEGRO_ALIGN_CENTRE, "(7 letras)");

                    //Botao de muito dificil
                    al_draw_filled_rounded_rectangle(755, 600, 1045, 780, 15, 15, al_map_rgb(0, 0, 115));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 900, 640, ALLEGRO_ALIGN_CENTRE, "Muito dificil");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 900, 680, ALLEGRO_ALIGN_CENTRE, "(8 letras)");

                }
            }

            //Tela de jogo
            if (pag_jogo) {

                //Pintando a tela de azul
                al_clear_to_color(al_map_rgb(0, 0, 115));

                //Escrevendo o nome do jogo
                al_draw_text(arial_titulo, al_map_rgb(0, 0, 0), TAM_X_TELA / 2, 25, ALLEGRO_ALIGN_CENTRE, "MISSING WORD");

                //Desenho das bolinhas das palavras que serao chutadas
                if (numero_letras_palavra_certa == 5) {

                    if ((contador_palavra - 1) == 1) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[0][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 341 && j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 341 && j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0.9, 0.9, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(470 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 2) {
                        for (int i = 0; i < 5; i++) {
                            cor_bolinhas[1][i] = cor_letras[i];
                        }
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(470 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 3) {
                        for (int i = 0; i < 5; i++) {
                            cor_bolinhas[2][i] = cor_letras[i];
                        }
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(470 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 4) {
                        for (int i = 0; i < 5; i++) {
                            cor_bolinhas[3][i] = cor_letras[i];
                        }
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(470 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }

                    }

                    if ((contador_palavra - 1) == 5) {
                        for (int i = 0; i < 5; i++) {
                            cor_bolinhas[4][i] = cor_letras[i];
                        }
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(470 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 6) {
                        for (int i = 0; i < 5; i++) {
                            cor_bolinhas[5][i] = cor_letras[i];
                        }
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 341, j < 5; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(470 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                
                }
                else if (numero_letras_palavra_certa == 6) {

                    if ((contador_palavra - 1) == 1) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[0][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(422 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 2) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[1][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(422 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 3) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[2][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(422 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 4) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[3][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(422 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 5) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[4][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(422 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 6) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[5][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 426 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(422 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                }
                else if (numero_letras_palavra_certa == 7) {
              
                    if ((contador_palavra - 1) == 1) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[0][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(385 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 2) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[1][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(385 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 3) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[2][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(385 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 4) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[3][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(385 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 5) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[4][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(385 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 6) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[5][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 511 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(385 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }


                }
                else if (numero_letras_palavra_certa == 8) {


                    if ((contador_palavra - 1) == 1) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[0][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[0][j] == 0) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[0][j] == 1) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[0][j] == 2) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[0][j] == 3) {
                                al_draw_filled_circle(342 + i, 135, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 2) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[1][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[1][j] == 0) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[1][j] == 1) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[1][j] == 2) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[1][j] == 3) {
                                al_draw_filled_circle(342 + i, 220, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 3) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[2][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[2][j] == 0) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[2][j] == 1) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[2][j] == 2) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[2][j] == 3) {
                                al_draw_filled_circle(342 + i, 305, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 4) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[3][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[3][j] == 0) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[3][j] == 1) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[3][j] == 2) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[3][j] == 3) {
                                al_draw_filled_circle(342 + i, 390, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 5) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[4][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[4][j] == 0) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[4][j] == 1) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[4][j] == 2) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[4][j] == 3) {
                                al_draw_filled_circle(342 + i, 475, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }

                    if ((contador_palavra - 1) == 6) {
                        for (int i = 0; i < numero_letras_palavra_certa; i++) {
                            cor_bolinhas[5][i] = cor_letras[i];
                        }

                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }
                    else {
                        for (int i = 0, j = 0; i < 596 && j < numero_letras_palavra_certa; i = i + 85, j++) {
                            if (cor_bolinhas[5][j] == 0) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0, 0, 0.9, 0.9)); //azul
                            }

                            if (cor_bolinhas[5][j] == 1) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0.9, 0, 0, 0.9)); //vermelho
                            }

                            if (cor_bolinhas[5][j] == 2) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));  //amarelo
                            }

                            if (cor_bolinhas[5][j] == 3) {
                                al_draw_filled_circle(342 + i, 560, 35, al_map_rgba_f(0, 0.9, 0, 0.9)); //verde
                            }
                        }
                    }


                }
            

                //Desenho das bolinhas da primeira linha do teclado (composta por 10 letras)
                for (int i = 0; i < 766; i = i + 85) {
                    //Letra Q
                    if (i == (85 * 0) && alfabeto_bolas[16] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == 0 && alfabeto_bolas[16] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == 0 && alfabeto_bolas[16] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == 0 && alfabeto_bolas[16] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra W
                    if (i == (85 * 1) && alfabeto_bolas[22] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[22] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[22] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[22] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra E
                    if (i == (85 * 2) && alfabeto_bolas[4] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[4] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[4] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[4] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra R
                    if (i == (85 * 3) && alfabeto_bolas[17] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[17] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[17] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[17] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra T
                    if (i == (85 * 4) && alfabeto_bolas[19] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[19] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[19] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[19] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra Y
                    if (i == (85 * 5) && alfabeto_bolas[24] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[24] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[24] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[24] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra U
                    if (i == (85 * 6) && alfabeto_bolas[20] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[20] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[20] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[20] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra I
                    if (i == (85 * 7) && alfabeto_bolas[8] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 7) && alfabeto_bolas[8] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 7) && alfabeto_bolas[8] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 7) && alfabeto_bolas[8] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra O
                    if (i == (85 * 8) && alfabeto_bolas[14] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 8) && alfabeto_bolas[14] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 8) && alfabeto_bolas[14] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 8) && alfabeto_bolas[14] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra P
                    if (i == (85 * 9) && alfabeto_bolas[15] == 0) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 9) && alfabeto_bolas[15] == 1) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 9) && alfabeto_bolas[15] == 2) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 9) && alfabeto_bolas[15] == 3) {
                        al_draw_filled_circle(265 + i, 700, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }
                


                }

                //Desenho das bolinhas da segunda linha do teclado (composta por 9 letras)
                for (int i = 0; i < 681; i = i + 85) {
                    //Letra A
                    if (i == (85 * 0) && alfabeto_bolas[0] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 0) && alfabeto_bolas[0] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 0) && alfabeto_bolas[0] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 0) && alfabeto_bolas[0] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra S
                    if (i == (85 * 1) && alfabeto_bolas[18] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[18] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[18] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[18] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra D
                    if (i == (85 * 2) && alfabeto_bolas[3] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[3] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[3] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[3] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra F
                    if (i == (85 * 3) && alfabeto_bolas[5] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[5] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[5] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[5] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra G
                    if (i == (85 * 4) && alfabeto_bolas[6] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[6] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[6] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[6] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra H
                    if (i == (85 * 5) && alfabeto_bolas[7] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[7] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[7] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[7] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra J
                    if (i == (85 * 6) && alfabeto_bolas[9] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[9] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[9] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[9] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra K
                    if (i == (85 * 7) && alfabeto_bolas[10] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 7) && alfabeto_bolas[10] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 7) && alfabeto_bolas[10] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 7) && alfabeto_bolas[10] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra L
                    if (i == (85 * 8) && alfabeto_bolas[11] == 0) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 8) && alfabeto_bolas[11] == 1) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 8) && alfabeto_bolas[11] == 2) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 8) && alfabeto_bolas[11] == 3) {
                        al_draw_filled_circle(230 + i, 785, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }



                }

                //Desenho das bolinhas da terceira linha do teclado (composta por 7 letras)
                for (int i = 0; i < 511; i = i + 85) {
                    if (i == (85 * 0) && alfabeto_bolas[25] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 0) && alfabeto_bolas[25] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 0) && alfabeto_bolas[25] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 0) && alfabeto_bolas[25] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra X
                    if (i == (85 * 1) && alfabeto_bolas[23] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[23] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[23] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 1) && alfabeto_bolas[23] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra C
                    if (i == (85 * 2) && alfabeto_bolas[2] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[2] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[2] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 2) && alfabeto_bolas[2] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra V
                    if (i == (85 * 3) && alfabeto_bolas[21] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[21] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[21] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 3) && alfabeto_bolas[21] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra B
                    if (i == (85 * 4) && alfabeto_bolas[1] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[1] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[1] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 4) && alfabeto_bolas[1] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra N
                    if (i == (85 * 5) && alfabeto_bolas[13] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[13] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[13] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 5) && alfabeto_bolas[13] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }

                    //Letra M
                    if (i == (85 * 6) && alfabeto_bolas[12] == 0) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0, 0.9, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[12] == 1) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0, 0, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[12] == 2) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0.9, 0.8, 0, 0.9));
                    }
                    else if (i == (85 * 6) && alfabeto_bolas[12] == 3) {
                        al_draw_filled_circle(265 + i, 870, 35, al_map_rgba_f(0, 0.9, 0, 0.9));
                    }
                }
            
                //Primeira linha do teclado
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 265, 677, ALLEGRO_ALIGN_CENTRE, "Q");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 350, 677, ALLEGRO_ALIGN_CENTRE, "W");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 435, 677, ALLEGRO_ALIGN_CENTRE, "E");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 520, 677, ALLEGRO_ALIGN_CENTRE, "R");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 605, 677, ALLEGRO_ALIGN_CENTRE, "T");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 690, 677, ALLEGRO_ALIGN_CENTRE, "Y");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 775, 677, ALLEGRO_ALIGN_CENTRE, "U");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 860, 677, ALLEGRO_ALIGN_CENTRE, "I");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 945, 677, ALLEGRO_ALIGN_CENTRE, "O");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 1030, 677, ALLEGRO_ALIGN_CENTRE, "P");

                //Segunda linha do teclado
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 230, 762, ALLEGRO_ALIGN_CENTRE, "A");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 315, 762, ALLEGRO_ALIGN_CENTRE, "S");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 400, 762, ALLEGRO_ALIGN_CENTRE, "D");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 485, 762, ALLEGRO_ALIGN_CENTRE, "F");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 570, 762, ALLEGRO_ALIGN_CENTRE, "G");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 655, 762, ALLEGRO_ALIGN_CENTRE, "H");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 740, 762, ALLEGRO_ALIGN_CENTRE, "J");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 825, 762, ALLEGRO_ALIGN_CENTRE, "K");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 910, 762, ALLEGRO_ALIGN_CENTRE, "L");

                //Terceira linha do teclado
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 265, 847, ALLEGRO_ALIGN_CENTRE, "Z");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 350, 847, ALLEGRO_ALIGN_CENTRE, "X");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 435, 847, ALLEGRO_ALIGN_CENTRE, "C");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 520, 847, ALLEGRO_ALIGN_CENTRE, "V");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 605, 847, ALLEGRO_ALIGN_CENTRE, "B");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 690, 847, ALLEGRO_ALIGN_CENTRE, "N");
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 775, 847, ALLEGRO_ALIGN_CENTRE, "M");

                //Botão delete
                al_draw_filled_rounded_rectangle(995, 750, 1140, 820, 15, 15, al_map_rgba_f(0, 0, 0.9, 0.9));
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 1068, 760, ALLEGRO_ALIGN_CENTER, "Apagar");

                //Botão enter
                al_draw_filled_rounded_rectangle(860, 835, 1065, 905, 15, 15, al_map_rgba_f(0, 0, 0.9, 0.9));
                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 963, 850, ALLEGRO_ALIGN_CENTER, "Enter");
  
                //impressão do botao de voltar
                al_draw_filled_rounded_rectangle(50, 50, 120, 120, 15, 15, al_map_rgba_f(0, 0, 0.9, 1));
                al_draw_text(arial_titulo, al_map_rgb(0, 0, 0), 85, 60, ALLEGRO_ALIGN_CENTER, "<");

                //Colocando as letras dos chutes
                for (int g = 1; g < 7; g++) {
                    for (int q = 0; q < numero_letras_palavra_certa; q++) { // Garante no máximo 5 letras
                        char letra = letras_das_palavras[g][q];
                        if (letra >= 'A' && letra <= 'Z') { // Garante que é uma letra válida
                            char texto[2] = { letra, '\0' }; // Converte a letra em string para o Allegro
                            if (numero_letras_palavra_certa == 5) {
                                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 470 + q * 85, 115 + (g - 1) * 85, ALLEGRO_ALIGN_CENTRE, texto);
                            }
                            else if (numero_letras_palavra_certa == 6) {
                                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 422 + q * 85, 115 + (g - 1) * 85, ALLEGRO_ALIGN_CENTRE, texto);
                            }
                            else if (numero_letras_palavra_certa == 7) {
                                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 385 + q * 85, 115 + (g - 1) * 85, ALLEGRO_ALIGN_CENTRE, texto);
                            }
                            else if (numero_letras_palavra_certa == 8) {
                                al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 342 + q * 85, 115 + (g - 1) * 85, ALLEGRO_ALIGN_CENTRE, texto);
                            }
                        }
                    }
                }

                //Tela do ganhou
                if (ganhou) {
                    al_draw_filled_rounded_rectangle(385, 100, 895, 595, 15, 15, al_map_rgba_f(0, 0, 0.9, 1));

                    //Escrita do "voce ganhou"
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 130, ALLEGRO_ALIGN_CENTER, "Parabens!");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 180, ALLEGRO_ALIGN_CENTER, "Voce ganhou! :)");

                    //Quadrado e escrita "jogar novamente" 
                    al_draw_filled_rounded_rectangle(405, 300, 630, 500, 15, 15, al_map_rgb(0, 0, 95));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 518, 350, ALLEGRO_ALIGN_CENTER, "Jogar");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 518, 390, ALLEGRO_ALIGN_CENTER, "Novamente");

                    //Quadrado e escrita "tela inicial"
                    al_draw_filled_rounded_rectangle(650, 300, 875, 500, 15, 15, al_map_rgb(0, 0, 95));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 763, 350, ALLEGRO_ALIGN_CENTER, "Pagina");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 763, 390, ALLEGRO_ALIGN_CENTER, "Inicial");

                    if (jogar_novamente) {
                        al_draw_filled_circle(800, 50, 25, al_map_rgba_f(0.9, 0, 0.9, 0.9));
                    }
                }

                //Tela do perdeu
                if (!ganhou && contador_palavra == 7) {
                    al_draw_filled_rounded_rectangle(385, 100, 895, 595, 15, 15, al_map_rgba_f(0, 0, 0.9, 1));

                    //Escrita do "voce perdeu"
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 130, ALLEGRO_ALIGN_CENTER, "Nao foi dessa vez :(");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 640, 180, ALLEGRO_ALIGN_CENTER, "Tente Novamente");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 660, 230, ALLEGRO_ALIGN_RIGHT, "A palavra era: ");
                    char temp[8] = { '\0'};
                    for (int i = 0; i < numero_letras_palavra_certa; i++) {
                        temp[i] = resposta.letras.itens[i];
                    }
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 760, 230, ALLEGRO_ALIGN_CENTER, temp);

                    //Quadrado e escrita "jogar novamente" 
                    al_draw_filled_rounded_rectangle(405, 300, 630, 500, 15, 15, al_map_rgb(0, 0, 95));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 518, 350, ALLEGRO_ALIGN_CENTER, "Jogar");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 518, 390, ALLEGRO_ALIGN_CENTER, "Novamente");

                    //Quadrado e escrita "tela inicial"
                    al_draw_filled_rounded_rectangle(650, 300, 875, 500, 15, 15, al_map_rgb(0, 0, 95));
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 763, 350, ALLEGRO_ALIGN_CENTER, "Pagina");
                    al_draw_text(arial_letras, al_map_rgb(0, 0, 0), 763, 390, ALLEGRO_ALIGN_CENTER, "Inicial");

                    if (jogar_novamente) {
                        al_draw_filled_circle(800, 50, 25, al_map_rgba_f(0.9, 0, 0.9, 0.9));
                    }
                }
            }

            al_flip_display();

            redraw = false;
        }

    }

    al_destroy_font(arial_letras);
    al_destroy_font(arial_titulo);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
