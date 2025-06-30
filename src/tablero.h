// src/tablero.h
#ifndef TABLERO_H
#define TABLERO_H

#define TAM_TABLERO 8
#define TAM_CASILLA 80
#define VACIO ' '
#define PEON_B 'P'
#define CABALLO_B 'N'
#define ALFIL_B 'B'
#define TORRE_B 'R'
#define REINA_B 'Q'
#define REY_B 'K'
#define PEON_N 'p'
#define CABALLO_N 'n'
#define ALFIL_N 'b'
#define TORRE_N 'r'
#define REINA_N 'q'
#define REY_N 'k'

typedef struct {
    char pieza;
    int x, y;
} Casilla;

typedef struct {
    Casilla tablero[TAM_TABLERO][TAM_TABLERO];
    int turno;
    int whiteKingMoved;
    int whiteRookH8Moved;
    int whiteRookA8Moved;
    int blackKingMoved;
    int blackRookH8Moved;
    int blackRookA8Moved;
    int moveCount;
    char posiciones[100][TAM_TABLERO][TAM_TABLERO];
} Juego;

void inicializarTablero(Juego* juego);
int validarMovimiento(Juego* juego, int fromX, int fromY, int toX, int toY);
int enJaque(Juego* juego, int turno);
int esJaqueMate(Juego* juego);
int esEmpate(Juego* juego);

#endif