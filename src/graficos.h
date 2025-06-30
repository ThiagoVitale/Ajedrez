// src/graficos.h
#ifndef GRAFICOS_H
#define GRAFICOS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "tablero.h"
#include "movimientos.h"

#define TAM_CASILLA 80

typedef struct {
    SDL_Window* ventana;
    SDL_Renderer* renderizador;
    SDL_Texture* texturasPiezas[12];
    TTF_Font* fuente;
    SDL_Texture* texturaTurnoBlancas;
    SDL_Texture* texturaTurnoNegras;
    SDL_Texture* texturaJaque;
    SDL_Texture* texturasHistorial[5];
    SDL_Rect rectTurno;
    SDL_Rect rectJaque;
    SDL_Rect rectHistorial[5];
    int historialCount;
} Graficos;

void inicializarGraficos(Graficos* graficos);
void cargarTexturas(Graficos* graficos);
void dibujarTablero(Graficos* graficos, Juego* juego, int arrastrando, char pieza, int mouseX, int mouseY, Movimiento* historial, int enJaque, int resaltarMovimientos);
void limpiarGraficos(Graficos* graficos);

#endif