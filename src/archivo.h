// src/archivo.h
#ifndef ARCHIVO_H
#define ARCHIVO_H

#include "tablero.h"
#include "movimientos.h"

void guardarPartida(Juego* juego, Movimiento* historial, const char* archivo);
void cargarPartida(Juego* juego, Movimiento** historial, const char* archivo);

#endif