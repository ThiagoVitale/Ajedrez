// src/archivo.c
#include "archivo.h"
#include <stdio.h>

void guardarPartida(Juego* juego, Movimiento* historial, const char* archivo) {
    FILE* f = fopen(archivo, "w");
    if (!f) return;
    // Guardar turno
    fprintf(f, "%d\n", juego->turno);
    // Guardar historial de movimientos
    Movimiento* actual = historial;
    while (actual) {
        fprintf(f, "%c %d %d %d %d %c\n", actual->pieza, actual->fromX, actual->fromY,
                actual->toX, actual->toY, actual->capturada);
        actual = actual->siguiente;
    }
    fclose(f);
}

void cargarPartida(Juego* juego, Movimiento** historial, const char* archivo) {
    FILE* f = fopen(archivo, "r");
    if (!f) return;
    // Reiniciar juego
    inicializarTablero(juego);
    liberarMovimientos(*historial);
    *historial = NULL;
    // Leer turno
    fscanf(f, "%d\n", &juego->turno);
    // Leer movimientos y reconstruir
    char pieza, capturada;
    int fromX, fromY, toX, toY;
    while (fscanf(f, "%c %d %d %d %d %c\n", &pieza, &fromX, &fromY, &toX, &toY, &capturada) == 6) {
        agregarMovimiento(historial, pieza, fromX, fromY, toX, toY, capturada);
        // Aplicar movimiento al tablero
        juego->tablero[toX][toY].pieza = pieza;
        juego->tablero[fromX][fromY].pieza = VACIO;
    }
    fclose(f);
}