// src/main.c
#include <SDL2/SDL.h>
#include "tablero.h"
#include "graficos.h"
#include "movimientos.h"
#include "ia.h"
#include "archivo.h"
#include <time.h>

int main(int argc, char* argv[]) {
    srand(time(NULL)); // Inicializar aleatorización
    Juego juego;
    Graficos graficos;
    Movimiento* historial = NULL;
    Pila* pila = NULL;
    ColaMov* cola = NULL;
    Capturadas capturadas;
    inicializarTablero(&juego);
    inicializarGraficos(&graficos);
    cargarTexturas(&graficos);
    inicializarCapturadas(&capturadas);

    int seleccionado = 0;
    int arrastrando = 0;
    int fromX = -1, fromY = -1;
    char piezaArrastrada = VACIO;
    int mouseX = 0, mouseY = 0;
    int modoIA = 1;
    int corriendo = 1;
    int resaltarMovimientos = 1; // Habilitar resaltado por defecto
    SDL_Event evento;
    Uint32 ultimaActualizacion = SDL_GetTicks();

    while (corriendo) {
        int enJaqueFlag = enJaque(&juego, juego.turno);
        if (esJaqueMate(&juego)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Fin del Juego",
                                     juego.turno == 0 ? "Jaque Mate: Ganan las Negras" : "Jaque Mate: Ganan las Blancas", graficos.ventana);
            corriendo = 0;
            break;
        }
        if (esEmpate(&juego)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Fin del Juego", "Empate", graficos.ventana);
            corriendo = 0;
            break;
        }

        // Procesar eventos
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                corriendo = 0;
                break;
            } else if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT) {
                int x = evento.button.x / TAM_CASILLA;
                int y = evento.button.y / TAM_CASILLA;
                if (!seleccionado && !arrastrando) {
                    if (juego.tablero[y][x].pieza != VACIO &&
                        ((juego.turno == 0 && juego.tablero[y][x].pieza >= 'A' && juego.tablero[y][x].pieza <= 'Z') ||
                         (juego.turno == 1 && juego.tablero[y][x].pieza >= 'a' && juego.tablero[y][x].pieza <= 'z'))) {
                        seleccionado = 1;
                        arrastrando = 1;
                        fromX = y;
                        fromY = x;
                        piezaArrastrada = juego.tablero[y][x].pieza;
                        mouseX = evento.button.x;
                        mouseY = evento.button.y;
                    }
                }
            } else if (evento.type == SDL_MOUSEMOTION && arrastrando) {
                mouseX = evento.motion.x;
                mouseY = evento.motion.y;
            } else if (evento.type == SDL_MOUSEBUTTONUP && evento.button.button == SDL_BUTTON_LEFT && arrastrando) {
                int x = evento.button.x / TAM_CASILLA;
                int y = evento.button.y / TAM_CASILLA;
                if (validarMovimiento(&juego, fromX, fromY, y, x)) {
                    pushPila(&pila, juego.tablero);
                    char capturada = juego.tablero[y][x].pieza;
                    agregarMovimiento(&historial, juego.tablero[fromX][fromY].pieza, fromX, fromY, y, x, capturada);
                    if (capturada != VACIO) {
                        agregarCapturada(&capturadas, capturada);
                    }
                    // Manejar enroque
                    if (juego.tablero[fromX][fromY].pieza == REY_B && fromY == 4 && (x == 6 || x == 2)) {
                        juego.whiteKingMoved = 1;
                        if (x == 6) {
                            juego.tablero[fromX][5].pieza = juego.tablero[fromX][7].pieza;
                            juego.tablero[fromX][7].pieza = VACIO;
                            juego.whiteRookH8Moved = 1;
                        } else if (x == 2) {
                            juego.tablero[fromX][3].pieza = juego.tablero[fromX][0].pieza;
                            juego.tablero[fromX][0].pieza = VACIO;
                            juego.whiteRookA8Moved = 1;
                        }
                    }
                    // Actualizar banderas de enroque
                    if (juego.tablero[fromX][fromY].pieza == REY_B) juego.whiteKingMoved = 1;
                    if (fromX == 7 && fromY == 0 && juego.tablero[fromX][fromY].pieza == TORRE_B) juego.whiteRookA8Moved = 1;
                    if (fromX == 7 && fromY == 7 && juego.tablero[fromX][fromY].pieza == TORRE_B) juego.whiteRookH8Moved = 1;
                    // Manejar promoción de peón
                    if ((juego.tablero[fromX][fromY].pieza == PEON_B && y == 0) ||
                        (juego.tablero[fromX][fromY].pieza == PEON_N && y == 7)) {
                        juego.tablero[y][x].pieza = (juego.tablero[fromX][fromY].pieza == PEON_B) ? REINA_B : REINA_N;
                    } else {
                        juego.tablero[y][x].pieza = juego.tablero[fromX][fromY].pieza;
                    }
                    juego.tablero[fromX][fromY].pieza = VACIO;
                    // Guardar posición para repetición
                    for (int i = 0; i < TAM_TABLERO; i++) {
                        for (int j = 0; j < TAM_TABLERO; j++) {
                            juego.posiciones[juego.moveCount][i][j] = juego.tablero[i][j].pieza;
                        }
                    }
                    juego.moveCount++;
                    juego.turno = 1;
                    // Limpiar texturas del historial para forzar actualización
                    for (int i = 0; i < graficos.historialCount; i++) {
                        if (graficos.texturasHistorial[i]) {
                            SDL_DestroyTexture(graficos.texturasHistorial[i]);
                            graficos.texturasHistorial[i] = NULL;
                        }
                    }
                    graficos.historialCount = 0;
                }
                seleccionado = 0;
                arrastrando = 0;
                piezaArrastrada = VACIO;
            } else if (evento.type == SDL_KEYDOWN) {
                if (evento.key.keysym.sym == SDLK_z) {
                    popPila(&pila, juego.tablero);
                    if (capturadas.tam > 0) capturadas.tam--;
                    juego.turno = 1 - juego.turno;
                    if (juego.moveCount > 0) juego.moveCount--;
                    // Limpiar texturas del historial para forzar actualización
                    for (int i = 0; i < graficos.historialCount; i++) {
                        if (graficos.texturasHistorial[i]) {
                            SDL_DestroyTexture(graficos.texturasHistorial[i]);
                            graficos.texturasHistorial[i] = NULL;
                        }
                    }
                    graficos.historialCount = 0;
                } else if (evento.key.keysym.sym == SDLK_s) {
                    guardarPartida(&juego, historial, "partida.txt");
                } else if (evento.key.keysym.sym == SDLK_l) {
                    cargarPartida(&juego, &historial, "partida.txt");
                    // Limpiar texturas del historial para forzar actualización
                    for (int i = 0; i < graficos.historialCount; i++) {
                        if (graficos.texturasHistorial[i]) {
                            SDL_DestroyTexture(graficos.texturasHistorial[i]);
                            graficos.texturasHistorial[i] = NULL;
                        }
                    }
                    graficos.historialCount = 0;
                } else if (evento.key.keysym.sym == SDLK_r) {
                    resaltarMovimientos = !resaltarMovimientos; // Alternar resaltado con 'R'
                }
            }
        }

        // Dibujar antes del movimiento de la IA para mostrar "Turno: Negras"
        if (modoIA && juego.turno == 1 && corriendo) {
            dibujarTablero(&graficos, &juego, arrastrando, piezaArrastrada, mouseX, mouseY, historial, enJaqueFlag, resaltarMovimientos);
            SDL_Delay(200);
            int fx, fy, tx, ty;
            generarMovimientoIA(&juego, &fx, &fy, &tx, &ty, &cola);
            if (fx != -1 && validarMovimiento(&juego, fx, fy, tx, ty)) {
                pushPila(&pila, juego.tablero);
                char capturada = juego.tablero[tx][ty].pieza;
                agregarMovimiento(&historial, juego.tablero[fx][fy].pieza, fx, fy, tx, ty, capturada);
                if (capturada != VACIO) {
                    agregarCapturada(&capturadas, capturada);
                }
                // Manejar enroque para la IA
                if (juego.tablero[fx][fy].pieza == REY_N && fy == 4 && (tx == 6 || tx == 2)) {
                    juego.blackKingMoved = 1;
                    if (tx == 6) {
                        juego.tablero[fx][5].pieza = juego.tablero[fx][7].pieza;
                        juego.tablero[fx][7].pieza = VACIO;
                        juego.blackRookH8Moved = 1;
                    } else if (tx == 2) {
                        juego.tablero[fx][3].pieza = juego.tablero[fx][0].pieza;
                        juego.tablero[fx][0].pieza = VACIO;
                        juego.blackRookA8Moved = 1;
                    }
                }
                // Actualizar banderas de enroque
                if (juego.tablero[fx][fy].pieza == REY_N) juego.blackKingMoved = 1;
                if (fx == 0 && fy == 0 && juego.tablero[fx][fy].pieza == TORRE_N) juego.blackRookA8Moved = 1;
                if (fx == 0 && fy == 7 && juego.tablero[fx][fy].pieza == TORRE_N) juego.blackRookH8Moved = 1;
                // Manejar promoción de peón para la IA
                if (juego.tablero[fx][fy].pieza == PEON_N && tx == 7) {
                    juego.tablero[tx][ty].pieza = REINA_N;
                } else {
                    juego.tablero[tx][ty].pieza = juego.tablero[fx][fy].pieza;
                }
                juego.tablero[fx][fy].pieza = VACIO;
                // Guardar posición para repetición
                for (int i = 0; i < TAM_TABLERO; i++) {
                    for (int j = 0; j < TAM_TABLERO; j++) {
                        juego.posiciones[juego.moveCount][i][j] = juego.tablero[i][j].pieza;
                    }
                }
                juego.moveCount++;
                juego.turno = 0;
                // Limpiar texturas del historial para forzar actualización
                for (int i = 0; i < graficos.historialCount; i++) {
                    if (graficos.texturasHistorial[i]) {
                        SDL_DestroyTexture(graficos.texturasHistorial[i]);
                        graficos.texturasHistorial[i] = NULL;
                    }
                }
                graficos.historialCount = 0;
            } else {
                printf("Error: La IA no encontró un movimiento válido\n");
                juego.turno = 0;
            }
            liberarCola(cola);
            cola = NULL;
        }

        // Actualizar pantalla solo si han pasado 10ms (aprox. 100 FPS) durante arrastre, 16ms si no
        Uint32 ahora = SDL_GetTicks();
        Uint32 intervalo = arrastrando ? 10 : 16;
        if (ahora - ultimaActualizacion >= intervalo) {
            dibujarTablero(&graficos, &juego, arrastrando, piezaArrastrada, mouseX, mouseY, historial, enJaqueFlag, resaltarMovimientos);
            ultimaActualizacion = ahora;
        }
    }

    liberarMovimientos(historial);
    liberarPila(pila);
    liberarCola(cola);
    liberarCapturadas(&capturadas);
    limpiarGraficos(&graficos);
    return 0;
}