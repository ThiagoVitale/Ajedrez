// src/tablero.c
#include "tablero.h"
#include <string.h>
#include <stdlib.h>

void inicializarTablero(Juego* juego) {
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            juego->tablero[i][j].pieza = VACIO;
            juego->tablero[i][j].x = j * TAM_CASILLA;
            juego->tablero[i][j].y = i * TAM_CASILLA;
        }
    }
    // Piezas blancas
    juego->tablero[7][0].pieza = TORRE_B;
    juego->tablero[7][1].pieza = CABALLO_B;
    juego->tablero[7][2].pieza = ALFIL_B;
    juego->tablero[7][3].pieza = REINA_B;
    juego->tablero[7][4].pieza = REY_B;
    juego->tablero[7][5].pieza = ALFIL_B;
    juego->tablero[7][6].pieza = CABALLO_B;
    juego->tablero[7][7].pieza = TORRE_B;
    for (int j = 0; j < TAM_TABLERO; j++) {
        juego->tablero[6][j].pieza = PEON_B;
    }
    // Piezas negras
    juego->tablero[0][0].pieza = TORRE_N;
    juego->tablero[0][1].pieza = CABALLO_N;
    juego->tablero[0][2].pieza = ALFIL_N;
    juego->tablero[0][3].pieza = REINA_N;
    juego->tablero[0][4].pieza = REY_N;
    juego->tablero[0][5].pieza = ALFIL_N;
    juego->tablero[0][6].pieza = CABALLO_N;
    juego->tablero[0][7].pieza = TORRE_N;
    for (int j = 0; j < TAM_TABLERO; j++) {
        juego->tablero[1][j].pieza = PEON_N;
    }
    juego->turno = 0;
    juego->whiteKingMoved = 0;
    juego->whiteRookH8Moved = 0;
    juego->whiteRookA8Moved = 0;
    juego->blackKingMoved = 0;
    juego->blackRookH8Moved = 0;
    juego->blackRookA8Moved = 0;
    juego->moveCount = 0;
    memset(juego->posiciones, 0, sizeof(juego->posiciones));
}

int enJaque(Juego* juego, int turno) {
    int reyX = -1, reyY = -1;
    char rey = (turno == 0) ? REY_B : REY_N;
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            if (juego->tablero[i][j].pieza == rey) {
                reyX = i;
                reyY = j;
                break;
            }
        }
        if (reyX != -1) break;
    }
    if (reyX == -1) return 0;

    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            char p = juego->tablero[i][j].pieza;
            if ((turno == 0 && p >= 'a' && p <= 'z') || (turno == 1 && p >= 'A' && p <= 'Z')) {
                if (validarMovimiento(juego, i, j, reyX, reyY)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int validarMovimiento(Juego* juego, int fromX, int fromY, int toX, int toY) {
    if (fromX < 0 || fromX >= TAM_TABLERO || fromY < 0 || fromY >= TAM_TABLERO ||
        toX < 0 || toX >= TAM_TABLERO || toY < 0 || toY >= TAM_TABLERO) {
        return 0;
    }

    char pieza = juego->tablero[fromX][fromY].pieza;
    char destino = juego->tablero[toX][toY].pieza;

    if ((juego->turno == 0 && (pieza < 'A' || pieza > 'Z')) ||
        (juego->turno == 1 && (pieza < 'a' || pieza > 'z'))) {
        return 0;
    }

    if (destino != VACIO) {
        if ((juego->turno == 0 && destino >= 'A' && destino <= 'Z') ||
            (juego->turno == 1 && destino >= 'a' && destino <= 'z')) {
            return 0;
        }
    }

    int dx = abs(toX - fromX);
    int dy = abs(toY - fromY);

    if (pieza == PEON_B || pieza == PEON_N) {
        int dir = (pieza == PEON_B) ? -1 : 1;
        if (fromY == toY && destino == VACIO && toX == fromX + dir) {
            return 1;
        }
        if (fromY == toY && destino == VACIO && toX == fromX + 2 * dir &&
            ((pieza == PEON_B && fromX == 6) || (pieza == PEON_N && fromX == 1)) &&
            juego->tablero[fromX + dir][fromY].pieza == VACIO) {
            return 1;
        }
        if (dy == 1 && toX == fromX + dir && destino != VACIO) {
            return 1;
        }
        return 0;
    }

    if (pieza == CABALLO_B || pieza == CABALLO_N) {
        if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
            return 1;
        }
        return 0;
    }

    if (pieza == ALFIL_B || pieza == ALFIL_N) {
        if (dx == dy) {
            int pasoX = (toX > fromX) ? 1 : -1;
            int pasoY = (toY > fromY) ? 1 : -1;
            for (int i = 1; i < dx; i++) {
                if (juego->tablero[fromX + i * pasoX][fromY + i * pasoY].pieza != VACIO) {
                    return 0;
                }
            }
            return 1;
        }
        return 0;
    }

    if (pieza == TORRE_B || pieza == TORRE_N) {
        if (fromX == toX) {
            int pasoY = (toY > fromY) ? 1 : -1;
            for (int y = fromY + pasoY; y != toY; y += pasoY) {
                if (juego->tablero[fromX][y].pieza != VACIO) {
                    return 0;
                }
            }
            return 1;
        } else if (fromY == toY) {
            int pasoX = (toX > fromX) ? 1 : -1;
            for (int x = fromX + pasoX; x != toX; x += pasoX) {
                if (juego->tablero[x][fromY].pieza != VACIO) {
                    return 0;
                }
            }
            return 1;
        }
        return 0;
    }

    if (pieza == REINA_B || pieza == REINA_N) {
        if (dx == dy) {
            int pasoX = (toX > fromX) ? 1 : -1;
            int pasoY = (toY > fromY) ? 1 : -1;
            for (int i = 1; i < dx; i++) {
                if (juego->tablero[fromX + i * pasoX][fromY + i * pasoY].pieza != VACIO) {
                    return 0;
                }
            }
            return 1;
        } else if (fromX == toX) {
            int pasoY = (toY > fromY) ? 1 : -1;
            for (int y = fromY + pasoY; y != toY; y += pasoY) {
                if (juego->tablero[fromX][y].pieza != VACIO) {
                    return 0;
                }
            }
            return 1;
        } else if (fromY == toY) {
            int pasoX = (toX > fromX) ? 1 : -1;
            for (int x = fromX + pasoX; x != toX; x += pasoX) {
                if (juego->tablero[x][fromY].pieza != VACIO) {
                    return 0;
                }
            }
            return 1;
        }
        return 0;
    }

    if (pieza == REY_B || pieza == REY_N) {
        if (dx <= 1 && dy <= 1) {
            return 1;
        }
        if (fromX == toX && fromY == 4 && (toY == 6 || toY == 2) &&
            ((pieza == REY_B && fromX == 7 && !juego->whiteKingMoved) ||
             (pieza == REY_N && fromX == 0 && !juego->blackKingMoved))) {
            if (toY == 6) { // Enroque corto
                if (!juego->tablero[fromX][5].pieza && !juego->tablero[fromX][6].pieza &&
                    juego->tablero[fromX][7].pieza == (pieza == REY_B ? TORRE_B : TORRE_N) &&
                    ((pieza == REY_B && !juego->whiteRookH8Moved) || (pieza == REY_N && !juego->blackRookH8Moved))) {
                    return !enJaque(juego, juego->turno);
                }
            } else if (toY == 2) { // Enroque largo
                if (!juego->tablero[fromX][3].pieza && !juego->tablero[fromX][2].pieza &&
                    !juego->tablero[fromX][1].pieza &&
                    juego->tablero[fromX][0].pieza == (pieza == REY_B ? TORRE_B : TORRE_N) &&
                    ((pieza == REY_B && !juego->whiteRookA8Moved) || (pieza == REY_N && !juego->blackRookA8Moved))) {
                    return !enJaque(juego, juego->turno);
                }
            }
        }
        return 0;
    }

    Casilla tableroTemporal[TAM_TABLERO][TAM_TABLERO];
    memcpy(tableroTemporal, juego->tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
    tableroTemporal[toX][toY].pieza = pieza;
    tableroTemporal[fromX][fromY].pieza = VACIO;
    Juego juegoTemporal = *juego;
    memcpy(juegoTemporal.tablero, tableroTemporal, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
    if (enJaque(&juegoTemporal, juego->turno)) {
        return 0;
    }

    return 1;
}

int esJaqueMate(Juego* juego) {
    if (!enJaque(juego, juego->turno)) {
        return 0;
    }
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            char p = juego->tablero[i][j].pieza;
            if ((juego->turno == 0 && p >= 'A' && p <= 'Z') ||
                (juego->turno == 1 && p >= 'a' && p <= 'z')) {
                for (int m = 0; m < TAM_TABLERO; m++) {
                    for (int n = 0; n < TAM_TABLERO; n++) {
                        if (validarMovimiento(juego, i, j, m, n)) {
                            Casilla tableroTemporal[TAM_TABLERO][TAM_TABLERO];
                            memcpy(tableroTemporal, juego->tablero, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                            tableroTemporal[m][n].pieza = p;
                            tableroTemporal[i][j].pieza = VACIO;
                            Juego juegoTemporal = *juego;
                            memcpy(juegoTemporal.tablero, tableroTemporal, TAM_TABLERO * TAM_TABLERO * sizeof(Casilla));
                            if (!enJaque(&juegoTemporal, juego->turno)) {
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return 1;
}

int esEmpate(Juego* juego) {
    // Empate por falta de movimientos legales (sin jaque)
    if (!enJaque(juego, juego->turno)) {
        int tieneMovimientos = 0;
        for (int i = 0; i < TAM_TABLERO; i++) {
            for (int j = 0; j < TAM_TABLERO; j++) {
                char p = juego->tablero[i][j].pieza;
                if ((juego->turno == 0 && p >= 'A' && p <= 'Z') ||
                    (juego->turno == 1 && p >= 'a' && p <= 'z')) {
                    for (int m = 0; m < TAM_TABLERO; m++) {
                        for (int n = 0; n < TAM_TABLERO; n++) {
                            if (validarMovimiento(juego, i, j, m, n)) {
                                tieneMovimientos = 1;
                                break;
                            }
                        }
                        if (tieneMovimientos) break;
                    }
                }
                if (tieneMovimientos) break;
            }
        }
        if (!tieneMovimientos) return 1;
    }

    // Empate por material insuficiente
    int piezasBlancas = 0, piezasNegras = 0;
    int alfilesBlancos = 0, alfilesNegros = 0;
    int caballosBlancos = 0, caballosNegros = 0;
    for (int i = 0; i < TAM_TABLERO; i++) {
        for (int j = 0; j < TAM_TABLERO; j++) {
            char p = juego->tablero[i][j].pieza;
            if (p == PEON_B || p == TORRE_B || p == REINA_B) piezasBlancas += 2;
            else if (p == CABALLO_B) caballosBlancos++;
            else if (p == ALFIL_B) alfilesBlancos++;
            else if (p == PEON_N || p == TORRE_N || p == REINA_N) piezasNegras += 2;
            else if (p == CABALLO_N) caballosNegros++;
            else if (p == ALFIL_N) alfilesNegros++;
        }
    }
    if (piezasBlancas == 0 && piezasNegras == 0 &&
        (caballosBlancos + alfilesBlancos <= 1) && (caballosNegros + alfilesNegros <= 1)) {
        return 1;
    }

    // Empate por repetición de posición
    if (juego->moveCount >= 2) {
        int repeticiones = 1;
        for (int i = juego->moveCount - 2; i >= 0; i -= 2) {
            int igual = 1;
            for (int x = 0; x < TAM_TABLERO && igual; x++) {
                for (int y = 0; y < TAM_TABLERO && igual; y++) {
                    if (juego->posiciones[juego->moveCount][x][y] != juego->posiciones[i][x][y]) {
                        igual = 0;
                    }
                }
            }
            if (igual) repeticiones++;
            if (repeticiones >= 3) return 1;
        }
    }

    return 0;
}