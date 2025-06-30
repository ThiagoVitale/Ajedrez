// src/ia.c
#include "ia.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>

typedef struct {
    int fromX, fromY, toX, toY;
    int score;
} MovimientoIA;

int evaluarPosicion(Juego* juego) {
    int score = 0;
    int valores[128] = {0};
    valores[PEON_B] = 100; valores[PEON_N] = -100;
    valores[CABALLO_B] = 320; valores[CABALLO_N] = -320;
    valores[ALFIL_B] = 330; valores[ALFIL_N] = -330;
    valores[TORRE_B] = 500; valores[TORRE_N] = -500;
    valores[REINA_B] = 900; valores[REINA_N] = -900;
    valores[REY_B] = 10000; valores[REY_N] = -10000;

    // Valor de las piezas
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            char p = juego->tablero[i][j].pieza;
            score += valores[(unsigned char)p];
            // Bonificación por control del centro
            if ((i == 3 || i == 4) && (j == 3 || j == 4)) {
                if (p >= 'A' && p <= 'Z') score += 20;
                else if (p >= 'a' && p <= 'z') score -= 20;
            }
            // Penalización por rey expuesto
            if (p == REY_B || p == REY_N) {
                int centroX = TAM_TABLERO / 2;
                int centroY = TAM_TABLERO / 2;
                int dist = abs(i - centroX) + abs(j - centroY);
                if (p == REY_B) score -= dist * 10;
                else score += dist * 10;
            }
            // Bonificación por desarrollar piezas en apertura
            if (juego->moveCount < 10) {
                if (p == CABALLO_B && (i == 6 && (j == 1 || j == 6))) score += 30; // Desarrollar caballos a f3/c3
                if (p == CABALLO_N && (i == 1 && (j == 1 || j == 6))) score -= 30; // Desarrollar caballos a f6/c6
                if (p == PEON_B && (i == 4 || i == 3) && (j == 3 || j == 4)) score += 25; // Peones centrales
                if (p == PEON_N && (i == 3 || i == 4) && (j == 3 || j == 4)) score -= 25; // Peones centrales
            }
        }
    }

    // Bonificación por movilidad
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            char p = juego->tablero[i][j].pieza;
            if ((p >= 'A' && p <= 'Z') || (p >= 'a' && p <= 'z')) {
                for (int m = 0; m < TAM_TABLERO; m++) {
                    for (int n = 0; n < TAM_TABLERO; n++) {
                        if (validarMovimiento(juego, i, j, m, n)) {
                            if (p >= 'A' && p <= 'Z') score += 3;
                            else score -= 3;
                        }
                    }
                }
            }
        }
    }

    // Penalización por repetir movimientos recientes
    if (juego->moveCount >= 2) {
        for (int m = juego->moveCount - 2; m < juego->moveCount; m++) {
            for (int i = 0; i < TAM_TABLERO; i++) {
                for (int j = 0; j < TAM_TABLERO; j++) {
                    if (juego->posiciones[m][i][j] != juego->tablero[i][j].pieza) {
                        if (juego->turno == 1) score -= 50; // Penalizar negras por repetir
                        else score += 50; // Penalizar blancas por repetir
                        break;
                    }
                }
            }
        }
    }

    return score;
}

int minimax(Juego* juego, int profundidad, int alpha, int beta, int maximizando) {
    if (profundidad == 0 || esJaqueMate(juego) || esEmpate(juego)) {
        return evaluarPosicion(juego);
    }

    if (maximizando) {
        int maxEval = -999999;
        for (int i = 0; i < TAM_TABLERO; i++) {
            for (int j = 0; j < TAM_TABLERO; j++) {
                char p = juego->tablero[i][j].pieza;
                if (p >= 'a' && p <= 'z') {
                    for (int m = 0; m < TAM_TABLERO; m++) {
                        for (int n = 0; n < TAM_TABLERO; n++) {
                            if (validarMovimiento(juego, i, j, m, n)) {
                                Casilla tableroTemporal[TAM_TABLERO][TAM_TABLERO];
                                memcpy(tableroTemporal, juego->tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                                tableroTemporal[m][n].pieza = p;
                                tableroTemporal[i][j].pieza = VACIO;
                                Juego juegoTemporal = *juego;
                                memcpy(juegoTemporal.tablero, tableroTemporal, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                                juegoTemporal.turno = 1 - juego->turno;
                                int eval = minimax(&juegoTemporal, profundidad - 1, alpha, beta, 0);
                                maxEval = eval > maxEval ? eval : maxEval;
                                alpha = alpha > eval ? alpha : eval;
                                if (beta <= alpha) return maxEval;
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (int i = 0; i < TAM_TABLERO; i++) {
            for (int j = 0; j < TAM_TABLERO; j++) {
                char p = juego->tablero[i][j].pieza;
                if (p >= 'A' && p <= 'Z') {
                    for (int m = 0; m < TAM_TABLERO; m++) {
                        for (int n = 0; n < TAM_TABLERO; n++) {
                            if (validarMovimiento(juego, i, j, m, n)) {
                                Casilla tableroTemporal[TAM_TABLERO][TAM_TABLERO];
                                memcpy(tableroTemporal, juego->tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                                tableroTemporal[m][n].pieza = p;
                                tableroTemporal[i][j].pieza = VACIO;
                                Juego juegoTemporal = *juego;
                                memcpy(juegoTemporal.tablero, tableroTemporal, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                                juegoTemporal.turno = 1 - juego->turno;
                                int eval = minimax(&juegoTemporal, profundidad - 1, alpha, beta, 1);
                                minEval = eval < minEval ? eval : minEval;
                                beta = beta < eval ? beta : eval;
                                if (beta <= alpha) return minEval;
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}

void generarMovimientoIA(Juego* juego, int* fromX, int* fromY, int* toX, int* toY, ColaMov** cola) {
    *fromX = -1;
    MovimientoIA movimientos[1000];
    int numMovimientos = 0;
    int profundidad = 2;

    // Recolectar todos los movimientos válidos
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            char p = juego->tablero[i][j].pieza;
            if (p >= 'a' && p <= 'z') {
                for (int m = 0; m < TAM_TABLERO; m++) {
                    for (int n = 0; n < TAM_TABLERO; n++) {
                        if (validarMovimiento(juego, i, j, m, n)) {
                            Casilla tableroTemporal[TAM_TABLERO][TAM_TABLERO];
                            memcpy(tableroTemporal, juego->tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                            tableroTemporal[m][n].pieza = p;
                            tableroTemporal[i][j].pieza = VACIO;
                            Juego juegoTemporal = *juego;
                            memcpy(juegoTemporal.tablero, tableroTemporal, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                            juegoTemporal.turno = 1 - juego->turno;
                            int puntaje = minimax(&juegoTemporal, profundidad - 1, -999999, 999999, 0);
                            movimientos[numMovimientos].fromX = i;
                            movimientos[numMovimientos].fromY = j;
                            movimientos[numMovimientos].toX = m;
                            movimientos[numMovimientos].toY = n;
                            movimientos[numMovimientos].score = puntaje;
                            numMovimientos++;
                        }
                    }
                }
            }
        }
    }

    // Seleccionar el mejor movimiento con aleatorización
    if (numMovimientos > 0) {
        int mejorPuntaje = -999999;
        for (int i = 0; i < numMovimientos; i++) {
            if (movimientos[i].score > mejorPuntaje) {
                mejorPuntaje = movimientos[i].score;
            }
        }
        int mejores[1000];
        int numMejores = 0;
        for (int i = 0; i < numMovimientos; i++) {
            if (movimientos[i].score >= mejorPuntaje - 20) { // Mayor tolerancia para variedad
                mejores[numMejores++] = i;
            }
        }
        int indice = mejores[rand() % numMejores];
        *fromX = movimientos[indice].fromX;
        *fromY = movimientos[indice].fromY;
        *toX = movimientos[indice].toX;
        *toY = movimientos[indice].toY;
    }

    liberarCola(*cola);
    *cola = NULL;

    // Retraso de 1.5 segundos
    SDL_Delay(1500);
}