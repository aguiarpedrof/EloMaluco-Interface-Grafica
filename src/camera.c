/**
 * @file camera.c
 * @brief Implementação da Câmera Virtual Orbital e Projeção Perspectiva
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#include "camera.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void camera_inicializar(Camera* cam) {
    cam->azimute = 35.0f;       /* Ângulo azimutal inicial */
    cam->elevacao = 25.0f;      /* Ângulo de elevação inicial */
    cam->distancia = 7.5f;      /* Distância de visualização */
    cam->alvoX = 0.0f;
    cam->alvoY = 0.0f;
    cam->alvoZ = 0.0f;
    cam->fov = 45.0f;
    cam->aspecto = 4.0f / 3.0f;
    cam->planoProximo = 0.1f;
    cam->planoDistante = 100.0f;
    cam->arrastando = false;
    cam->ultimoMouseX = 0;
    cam->ultimoMouseY = 0;
}

void camera_resetar(Camera* cam) {
    camera_inicializar(cam);
}

void camera_configurar_projecao(Camera* cam, int largura, int altura) {
    if (altura <= 0) altura = 1;
    cam->aspecto = (float)largura / (float)altura;

    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /* Configuração da projeção perspectiva conforme exigido */
    gluPerspective(cam->fov, cam->aspecto, cam->planoProximo, cam->planoDistante);
    
    glMatrixMode(GL_MODELVIEW);
}

void camera_aplicar_vista(const Camera* cam) {
    /* Conversão de coordenadas esféricas para coordenadas cartesianas */
    float radAzimute = cam->azimute * (float)M_PI / 180.0f;
    float radElevacao = cam->elevacao * (float)M_PI / 180.0f;

    float eyeX = cam->alvoX + cam->distancia * cosf(radElevacao) * sinf(radAzimute);
    float eyeY = cam->alvoY + cam->distancia * sinf(radElevacao);
    float eyeZ = cam->alvoZ + cam->distancia * cosf(radElevacao) * cosf(radAzimute);

    /* Câmera virtual: Define posição do observador, mira e vetor up */
    gluLookAt(
        eyeX, eyeY, eyeZ,            /* Posição da câmera (olho) */
        cam->alvoX, cam->alvoY, cam->alvoZ, /* Ponto para onde a câmera olha */
        0.0f, 1.0f, 0.0f             /* Vetor UP (orientação vertical) */
    );
}

void camera_mouse_click(Camera* cam, int botao, int estado, int x, int y) {
    /* Botão esquerdo do mouse ativa o arrasto orbital */
    if (botao == GLUT_LEFT_BUTTON) {
        if (estado == GLUT_DOWN) {
            cam->arrastando = true;
            cam->ultimoMouseX = x;
            cam->ultimoMouseY = y;
        } else if (estado == GLUT_UP) {
            cam->arrastando = false;
        }
    }
    
    /* Suporte a scroll do mouse no FreeGLUT (botão 3 = scroll up, 4 = scroll down) */
    if (estado == GLUT_DOWN) {
        if (botao == 3) {
            camera_zoom(cam, -0.4f);
        } else if (botao == 4) {
            camera_zoom(cam, 0.4f);
        }
    }
}

void camera_mouse_arrasto(Camera* cam, int x, int y) {
    if (!cam->arrastando) return;

    int dx = x - cam->ultimoMouseX;
    int dy = y - cam->ultimoMouseY;

    /* Sensibilidade de rotação */
    const float sensibilidade = 0.45f;
    cam->azimute += (float)dx * sensibilidade;
    cam->elevacao += (float)dy * sensibilidade;

    /* Normaliza azimute para [0, 360) */
    while (cam->azimute >= 360.0f) cam->azimute -= 360.0f;
    while (cam->azimute < 0.0f) cam->azimute += 360.0f;

    /* Limita elevação para evitar inversão da câmera (gimbal lock) */
    if (cam->elevacao > 85.0f) cam->elevacao = 85.0f;
    if (cam->elevacao < -85.0f) cam->elevacao = -85.0f;

    cam->ultimoMouseX = x;
    cam->ultimoMouseY = y;
}

void camera_zoom(Camera* cam, float delta) {
    cam->distancia += delta;
    /* Limites de zoom (escala de aproximação/afastamento) */
    if (cam->distancia < 3.2f) cam->distancia = 3.2f;
    if (cam->distancia > 18.0f) cam->distancia = 18.0f;
}
