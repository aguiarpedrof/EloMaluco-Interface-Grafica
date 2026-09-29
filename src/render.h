/**
 * @file render.h
 * @brief Modelagem geométrica 3D, materiais, iluminação e renderização da cena
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#ifndef RENDER_H
#define RENDER_H

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "puzzle.h"
#include "camera.h"

/* Inicialização de parâmetros de renderização, luzes e materiais */
void render_inicializar(void);

/* Desenho completo da cena 3D */
void render_desenhar_cena(const EloMaluco* elo, const Camera* cam, bool exibirHUD, int largura, int altura);

/* Modelagem geométrica dos componentes do puzzle */
void render_desenhar_torre_central(void);
void render_desenhar_base_e_topo(void);
void render_desenhar_peca_3d(const Peca* peca, int linha, int coluna, bool resolvido);
void render_desenhar_guia_trilhos(void);

/* Desenho da mesa de suporte com textura */
void render_desenhar_mesa(void);

/* HUD 2D em projeção ortográfica */
void render_desenhar_hud(const EloMaluco* elo, const Camera* cam, int largura, int altura);

#endif /* RENDER_H */
