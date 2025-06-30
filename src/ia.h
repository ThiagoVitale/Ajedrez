// src/ia.h
#ifndef IA_H
#define IA_H

#include "tablero.h"
#include "movimientos.h"

void generarMovimientoIA(Juego* juego, int* fromX, int* fromY, int* toX, int* toY, ColaMov** cola);

#endif