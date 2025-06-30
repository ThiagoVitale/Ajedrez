// src/movimientos.c
#include "movimientos.h"
#include <stdlib.h>
#include <string.h>

void inicializarCapturadas(Capturadas* capturadas) {
    capturadas->capacidad = 10;
    capturadas->tam = 0;
    capturadas->piezas = (char*)malloc(capturadas->capacidad * sizeof(char));
}

void agregarMovimiento(Movimiento** historial, char pieza, int fromX, int fromY, int toX, int toY, char capturada) {
    Movimiento* nuevo = (Movimiento*)malloc(sizeof(Movimiento));
    nuevo->pieza = pieza;
    nuevo->fromX = fromX;
    nuevo->fromY = fromY;
    nuevo->toX = toX;
    nuevo->toY = toY;
    nuevo->capturada = capturada;
    nuevo->siguiente = *historial;
    *historial = nuevo;
}

void pushPila(Pila** pila, Casilla tablero[TAM_TABLERO][TAM_TABLERO]) {
    Pila* nuevo = (Pila*)malloc(sizeof(Pila));
    memcpy(nuevo->tablero, tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
    nuevo->anterior = *pila;
    *pila = nuevo;
}

void popPila(Pila** pila, Casilla tablero[TAM_TABLERO][TAM_TABLERO]) {
    if (*pila) {
        Pila* temp = *pila;
        memcpy(tablero, temp->tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
        *pila = temp->anterior;
        free(temp);
    }
}

void encolarMovimiento(ColaMov** cola, int fromX, int fromY, int toX, int toY, int puntaje) {
    ColaMov* nuevo = (ColaMov*)malloc(sizeof(ColaMov));
    nuevo->fromX = fromX;
    nuevo->fromY = fromY;
    nuevo->toX = toX;
    nuevo->toY = toY;
    nuevo->puntaje = puntaje;
    nuevo->siguiente = NULL;
    if (!*cola) {
        *cola = nuevo;
    } else {
        ColaMov* actual = *cola;
        while (actual->siguiente) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
}

void desencolarMovimiento(ColaMov** cola, int* fromX, int* fromY, int* toX, int* toY) {
    if (*cola) {
        ColaMov* temp = *cola;
        *fromX = temp->fromX;
        *fromY = temp->fromY;
        *toX = temp->toX;
        *toY = temp->toY;
        *cola = temp->siguiente;
        free(temp);
    }
}

void agregarCapturada(Capturadas* capturadas, char pieza) {
    if (capturadas->tam >= capturadas->capacidad) {
        capturadas->capacidad *= 2;
        capturadas->piezas = (char*)realloc(capturadas->piezas, capturadas->capacidad * sizeof(char));
    }
    capturadas->piezas[capturadas->tam++] = pieza;
}

void liberarMovimientos(Movimiento* historial) {
    while (historial) {
        Movimiento* temp = historial;
        historial = historial->siguiente;
        free(temp);
    }
}

void liberarPila(Pila* pila) {
    while (pila) {
        Pila* temp = pila;
        pila = pila->anterior;
        free(temp);
    }
}

void liberarCola(ColaMov* cola) {
    while (cola) {
        ColaMov* temp = cola;
        cola = cola->siguiente;
        free(temp);
    }
}

void liberarCapturadas(Capturadas* capturadas) {
    free(capturadas->piezas);
    capturadas->tam = 0;
    capturadas->capacidad = 0;
}