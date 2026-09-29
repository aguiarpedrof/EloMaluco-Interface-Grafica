/**
 * @file camera.h
 * @brief Gerenciamento da Câmera Virtual Orbital e Projeção Perspectiva
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct {
    /* Coordenadas esféricas da câmera orbital */
    float azimute;      /* Rotação horizontal em graus (0 a 360) */
    float elevacao;     /* Rotação vertical em graus (-80 a 80) */
    float distancia;    /* Distância até o ponto focal (zoom/escala) */
    
    /* Ponto focal (alvo da câmera) */
    float alvoX, alvoY, alvoZ;
    
    /* Parâmetros de Projeção */
    float fov;          /* Campo de visão (Field of View) em graus */
    float aspecto;      /* Razão de aspecto (largura / altura) */
    float planoProximo; /* Near clipping plane */
    float planoDistante;/* Far clipping plane */
    
    /* Estado de interação com mouse */
    bool arrastando;
    int ultimoMouseX;
    int ultimoMouseY;
} Camera;

/* Inicialização e reset */
void camera_inicializar(Camera* cam);
void camera_resetar(Camera* cam);

/* Configuração da matriz de projeção perspectiva */
void camera_configurar_projecao(Camera* cam, int largura, int altura);

/* Aplicação da matriz de visualização gluLookAt */
void camera_aplicar_vista(const Camera* cam);

/* Interação com mouse */
void camera_mouse_click(Camera* cam, int botao, int estado, int x, int y);
void camera_mouse_arrasto(Camera* cam, int x, int y);
void camera_zoom(Camera* cam, float delta);

#endif /* CAMERA_H */
