// src/graficos.c
#include "graficos.h"
#include <stdio.h>

void inicializarGraficos(Graficos* graficos) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    graficos->ventana = SDL_CreateWindow("Ajedrez", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        TAM_TABLERO * TAM_CASILLA + 200, TAM_TABLERO * TAM_CASILLA, 0);
    graficos->renderizador = SDL_CreateRenderer(graficos->ventana, -1, SDL_RENDERER_ACCELERATED);
    graficos->fuente = TTF_OpenFont("assets/fuente.ttf", 20);
    if (!graficos->fuente) {
        printf("Error cargando fuente: %s\n", TTF_GetError());
    }
    graficos->texturaTurnoBlancas = NULL;
    graficos->texturaTurnoNegras = NULL;
    graficos->texturaJaque = NULL;
    for (int i = 0; i < 5; i++) {
        graficos->texturasHistorial[i] = NULL;
    }
    graficos->historialCount = 0;

    // Crear texturas de turno iniciales
    if (graficos->fuente) {
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* superficie = TTF_RenderText_Solid(graficos->fuente, "Turno: Blancas", color);
        graficos->texturaTurnoBlancas = SDL_CreateTextureFromSurface(graficos->renderizador, superficie);
        graficos->rectTurno = (SDL_Rect){TAM_TABLERO * TAM_CASILLA + 10, 10, superficie->w, superficie->h};
        SDL_FreeSurface(superficie);

        superficie = TTF_RenderText_Solid(graficos->fuente, "Turno: Negras", color);
        graficos->texturaTurnoNegras = SDL_CreateTextureFromSurface(graficos->renderizador, superficie);
        SDL_FreeSurface(superficie);

        superficie = TTF_RenderText_Solid(graficos->fuente, "¡Jaque!", color);
        graficos->texturaJaque = SDL_CreateTextureFromSurface(graficos->renderizador, superficie);
        graficos->rectJaque = (SDL_Rect){TAM_TABLERO * TAM_CASILLA + 10, 40, superficie->w, superficie->h};
        SDL_FreeSurface(superficie);
    }
}

void cargarTexturas(Graficos* graficos) {
    const char* nombresPiezas[] = {
        "rey_blanco.png", "reina_blanca.png", "alfil_blanco.png", "caballo_blanco.png", "torre_blanca.png", "peon_blanco.png",
        "rey_negro.png", "reina_negra.png", "alfil_negro.png", "caballo_negro.png", "torre_negra.png", "peon_negro.png"
    };
    char ruta[50];
    for (int i = 0; i < 12; i++) {
        snprintf(ruta, sizeof(ruta), "assets/%s", nombresPiezas[i]);
        printf("Intentando cargar: %s\n", ruta);
        graficos->texturasPiezas[i] = IMG_LoadTexture(graficos->renderizador, ruta);
        if (!graficos->texturasPiezas[i]) {
            printf("Error cargando %s: %s\n", ruta, IMG_GetError());
        }
    }
}

void dibujarTablero(Graficos* graficos, Juego* juego, int arrastrando, char pieza, int mouseX, int mouseY, Movimiento* historial, int enJaque, int resaltarMovimientos) {
    SDL_SetRenderDrawColor(graficos->renderizador, 0, 0, 0, 255);
    SDL_RenderClear(graficos->renderizador);

    // Dibujar casillas
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            SDL_Rect rect = {j * TAM_CASILLA, i * TAM_CASILLA, TAM_CASILLA, TAM_CASILLA};
            if ((i + j) % 2 == 0) {
                SDL_SetRenderDrawColor(graficos->renderizador, 255, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(graficos->renderizador, 139, 69, 19, 255);
            }
            SDL_RenderFillRect(graficos->renderizador, &rect);

            // Resaltar casilla seleccionada y movimientos válidos
            if (arrastrando && pieza != VACIO && resaltarMovimientos) {
                int fromX = mouseY / TAM_CASILLA;
                int fromY = mouseX / TAM_CASILLA;
                if (i == fromX && j == fromY) {
                    SDL_SetRenderDrawColor(graficos->renderizador, 0, 255, 0, 100);
                    SDL_RenderFillRect(graficos->renderizador, &rect);
                }
                if (validarMovimiento(juego, fromX, fromY, i, j)) {
                    SDL_SetRenderDrawColor(graficos->renderizador, 0, 255, 0, 50);
                    SDL_RenderFillRect(graficos->renderizador, &rect);
                }
            }

            // Dibujar pieza (excepto si está siendo arrastrada)
            char p = juego->tablero[i][j].pieza;
            if (arrastrando && p == pieza && i == mouseY / TAM_CASILLA && j == mouseX / TAM_CASILLA) {
                continue;
            }
            int indiceTextura = -1;
            if (p == PEON_B) indiceTextura = 5;
            else if (p == CABALLO_B) indiceTextura = 3;
            else if (p == ALFIL_B) indiceTextura = 2;
            else if (p == TORRE_B) indiceTextura = 4;
            else if (p == REINA_B) indiceTextura = 1;
            else if (p == REY_B) indiceTextura = 0;
            else if (p == PEON_N) indiceTextura = 11;
            else if (p == CABALLO_N) indiceTextura = 9;
            else if (p == ALFIL_N) indiceTextura = 8;
            else if (p == TORRE_N) indiceTextura = 10;
            else if (p == REINA_N) indiceTextura = 7;
            else if (p == REY_N) indiceTextura = 6;

            if (indiceTextura != -1 && graficos->texturasPiezas[indiceTextura]) {
                SDL_RenderCopy(graficos->renderizador, graficos->texturasPiezas[indiceTextura], NULL, &rect);
            }
        }
    }

    // Dibujar pieza arrastrada
    if (arrastrando && pieza != VACIO) {
        int indiceTextura = -1;
        if (pieza == PEON_B) indiceTextura = 5;
        else if (pieza == CABALLO_B) indiceTextura = 3;
        else if (pieza == ALFIL_B) indiceTextura = 2;
        else if (pieza == TORRE_B) indiceTextura = 4;
        else if (pieza == REINA_B) indiceTextura = 1;
        else if (pieza == REY_B) indiceTextura = 0;
        else if (pieza == PEON_N) indiceTextura = 11;
        else if (pieza == CABALLO_N) indiceTextura = 9;
        else if (pieza == ALFIL_N) indiceTextura = 8;
        else if (pieza == TORRE_N) indiceTextura = 10;
        else if (pieza == REINA_N) indiceTextura = 7;
        else if (pieza == REY_N) indiceTextura = 6;

        if (indiceTextura != -1 && graficos->texturasPiezas[indiceTextura]) {
            SDL_Rect rect = {mouseX - TAM_CASILLA / 2, mouseY - TAM_CASILLA / 2, TAM_CASILLA, TAM_CASILLA};
            SDL_RenderCopy(graficos->renderizador, graficos->texturasPiezas[indiceTextura], NULL, &rect);
        }
    }

    // Dibujar texto cacheado
    if (graficos->fuente) {
        // Turno
        SDL_Texture* texturaTurno = juego->turno == 0 ? graficos->texturaTurnoBlancas : graficos->texturaTurnoNegras;
        SDL_RenderCopy(graficos->renderizador, texturaTurno, NULL, &graficos->rectTurno);

        // Jaque
        if (enJaque) {
            SDL_RenderCopy(graficos->renderizador, graficos->texturaJaque, NULL, &graficos->rectJaque);
        }

        // Historial
        int y = 70;
        Movimiento* actual = historial;
        int count = 0;
        if (graficos->historialCount != 0) {
            for (int i = 0; i < graficos->historialCount && i < 5; i++) {
                if (graficos->texturasHistorial[i]) {
                    SDL_RenderCopy(graficos->renderizador, graficos->texturasHistorial[i], NULL, &graficos->rectHistorial[i]);
                }
            }
        }
        // Actualizar historial si cambió
        while (actual && count < 5) {
            char notacion[10];
            char piezaChar = actual->pieza == PEON_B || actual->pieza == PEON_N ? ' ' : actual->pieza;
            snprintf(notacion, sizeof(notacion), "%c%c%d-%c%d", piezaChar, 'a' + actual->fromY, 8 - actual->fromX,
                     'a' + actual->toY, 8 - actual->toX);
            if (count >= graficos->historialCount || !graficos->texturasHistorial[count]) {
                SDL_Color color = {255, 255, 255, 255};
                SDL_Surface* superficie = TTF_RenderText_Solid(graficos->fuente, notacion, color);
                graficos->texturasHistorial[count] = SDL_CreateTextureFromSurface(graficos->renderizador, superficie);
                graficos->rectHistorial[count] = (SDL_Rect){TAM_TABLERO * TAM_CASILLA + 10, y, superficie->w, superficie->h};
                SDL_FreeSurface(superficie);
                graficos->historialCount = count + 1;
            }
            y += 30;
            actual = actual->siguiente;
            count++;
        }
    }

    SDL_RenderPresent(graficos->renderizador);
}

void limpiarGraficos(Graficos* graficos) {
    for (int i = 0; i < 12; i++) {
        if (graficos->texturasPiezas[i]) {
            SDL_DestroyTexture(graficos->texturasPiezas[i]);
        }
    }
    if (graficos->texturaTurnoBlancas) SDL_DestroyTexture(graficos->texturaTurnoBlancas);
    if (graficos->texturaTurnoNegras) SDL_DestroyTexture(graficos->texturaTurnoNegras);
    if (graficos->texturaJaque) SDL_DestroyTexture(graficos->texturaJaque);
    for (int i = 0; i < 5; i++) {
        if (graficos->texturasHistorial[i]) {
            SDL_DestroyTexture(graficos->texturasHistorial[i]);
        }
    }
    if (graficos->fuente) {
        TTF_CloseFont(graficos->fuente);
    }
    SDL_DestroyRenderer(graficos->renderizador);
    SDL_DestroyWindow(graficos->ventana);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}