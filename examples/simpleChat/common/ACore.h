#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60
#define IP_MAX_SIZE 100

double startingTime;

ALLEGRO_DISPLAY *janela;
ALLEGRO_EVENT_QUEUE *fila_eventos;

//========================
//FONT AND BITMAP POINTERS
ALLEGRO_FONT *ubuntu;
ALLEGRO_FONT *start;

ALLEGRO_BITMAP* boi_1;
ALLEGRO_BITMAP* boi_2;
ALLEGRO_BITMAP* boi_3;
ALLEGRO_BITMAP* boi_4;
ALLEGRO_BITMAP* botao_1;
ALLEGRO_BITMAP* botao_2;
ALLEGRO_BITMAP* botao_3;
ALLEGRO_BITMAP* background;
ALLEGRO_BITMAP* seta;
ALLEGRO_BITMAP* intro;
ALLEGRO_BITMAP* fundoDeTela;
ALLEGRO_BITMAP* story;
ALLEGRO_BITMAP* boi_1_pequeno;
ALLEGRO_BITMAP* boi_2_pequeno;
ALLEGRO_BITMAP* boi_3_pequeno;
ALLEGRO_BITMAP* boi_4_pequeno;
ALLEGRO_BITMAP* boi_1_ataque;
ALLEGRO_BITMAP* boi_2_ataque;
ALLEGRO_BITMAP* boi_3_ataque;
ALLEGRO_BITMAP* boi_4_ataque;
ALLEGRO_BITMAP* vitoria;
ALLEGRO_BITMAP* derrota;
//========================
//========================

//EXAMPLE STRUCT
typedef struct DADOS
{
    char mensagem[100];
    int valor;
}DADOS;


//MAIN ALLEGRO FUNCTIONS
bool coreInit();
bool windowInit(int W, int H, char title[]);
bool inputInit();
void allegroEnd();

//FPS CONTROL FUNCTIONS
void startTimer();
double getTimer();
void FPSLimit();

//RESOURCE LOADING FUNCTIONS
bool loadGraphics();
bool fontInit();

//INPUT READING FUNCTION
void readInput(ALLEGRO_EVENT event, char str[], int limit);


#endif
