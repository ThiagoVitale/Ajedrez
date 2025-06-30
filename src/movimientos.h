// src/movimientos.h
#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "tablero.h"

typedef struct Movimiento {
    char pieza;
    int fromX, fromY, toX, toY;
    char capturada;
    struct Movimiento* siguiente;
} Movimiento;

typedef struct Pila {
    Casilla tablero[TAM_TABLERO][TAM_TABLERO];
    struct Pila* anterior;
} Pila;

typedef struct ColaMov {
    int fromX, fromY, toX, toY;
    int puntaje;
    struct ColaMov* siguiente;
} ColaMov;

typedef struct {
    char* piezas;
    int tam;
    int capacidad;
} Capturadas;

void inicializarCapturadas(Capturadas* capturadas);
void agregarMovimiento(Movimiento** historial, char pieza, int fromX, int fromY, int toX, int toY, char capturada);
void pushPila(Pila** pila, Casilla tablero[TAM_TABLERO][TAM_TABLERO]);
void popPila(Pila** pila, Casilla tablero[TAM_TABLERO][TAM_TABLERO]);
void encolarMovimiento(ColaMov** cola, int fromX, int fromY, int toX, int toY, int puntaje);
void desencolarMovimiento(ColaMov** cola, int* fromX, int* fromY, int* toX, int* toY);
void agregarCapturada(Capturadas* capturadas, char pieza);
void liberarMovimientos(Movimiento* historial);
void liberarPila(Pila* pila);
void liberarCola(ColaMov* cola);
void liberarCapturadas(Capturadas* capturadas);

#endif