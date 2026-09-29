/**
 * @file elo_maluco.c
 * @brief Aplicação Gráfica 3D Interativa do Elo Maluco (OpenGL / FreeGLUT)
 * Disciplina: ECOI24 - Computação Gráfica e PDI
 * Universidade Federal de Itajubá - UNIFEI
 * Professor: Dr. André Ribeiro de Brito
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "puzzle.h"
#include "camera.h"

/* Instâncias globais */
static EloMaluco g_elo;
static Camera g_camera;
static int g_larguraJanela = 1024;
static int g_alturaJanela = 768;
static bool g_exibirHUD = true;

/* Protótipos de callbacks */
static void callback_display(void);
static void callback_reshape(int w, int h);
static void callback_keyboard(unsigned char key, int x, int y);
static void callback_special(int key, int x, int y);
static void callback_mouse(int botao, int estado, int x, int y);
static void callback_motion(int x, int y);
static void callback_timer(int valor);

static void inicializar_opengl(void) {
    /* Teste de Profundidade para correta visualização e oclusão 3D */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    /* Cor de fundo elegante (estúdio/mesa de trabalho) */
    glClearColor(0.10f, 0.12f, 0.16f, 1.0f);

    /* Habilita normalização de normais para transformações com escala */
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

static void callback_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Aplica a câmera virtual (gluLookAt) */
    camera_aplicar_vista(&g_camera);

    /* Eixos de referência simples para orientação inicial */
    glBegin(GL_LINES);
        /* Eixo X: Vermelho */
        glColor3f(0.8f, 0.2f, 0.2f);
        glVertex3f(-2.0f, 0.0f, 0.0f);
        glVertex3f( 2.0f, 0.0f, 0.0f);
        /* Eixo Y: Verde */
        glColor3f(0.2f, 0.8f, 0.2f);
        glVertex3f(0.0f, -2.0f, 0.0f);
        glVertex3f(0.0f,  2.0f, 0.0f);
        /* Eixo Z: Azul */
        glColor3f(0.2f, 0.4f, 0.9f);
        glVertex3f(0.0f, 0.0f, -2.0f);
        glVertex3f(0.0f, 0.0f,  2.0f);
    glEnd();

    glutSwapBuffers();
}

static void callback_reshape(int w, int h) {
    g_larguraJanela = w;
    g_alturaJanela = h;
    camera_configurar_projecao(&g_camera, w, h);
}

static void callback_keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case 27: /* ESC */
            exit(0);
            break;
        case 'r':
        case 'R':
            puzzle_resetar(&g_elo);
            break;
        case 'e':
        case 'E':
            puzzle_embaralhar(&g_elo, 20);
            break;
        case 'd':
        case 'D':
            puzzle_rotacionar_superior_direita(&g_elo);
            break;
        case 'a':
        case 'A':
            puzzle_rotacionar_superior_esquerda(&g_elo);
            break;
        case 'l':
        case 'L':
            puzzle_rotacionar_inferior_direita(&g_elo);
            break;
        case 'j':
        case 'J':
            puzzle_rotacionar_inferior_esquerda(&g_elo);
            break;
        case 'w':
        case 'W':
            puzzle_mover_face_cima(&g_elo);
            break;
        case 's':
        case 'S':
            puzzle_mover_face_baixo(&g_elo);
            break;
        case '+':
        case '=':
            camera_zoom(&g_camera, -0.4f);
            break;
        case '-':
        case '_':
            camera_zoom(&g_camera, 0.4f);
            break;
        case 'h':
        case 'H':
            g_exibirHUD = !g_exibirHUD;
            break;
        case 'c':
        case 'C':
            camera_resetar(&g_camera);
            break;
    }
    glutPostRedisplay();
}

static void callback_special(int key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case GLUT_KEY_RIGHT:
            puzzle_rotacionar_superior_direita(&g_elo);
            break;
        case GLUT_KEY_LEFT:
            puzzle_rotacionar_superior_esquerda(&g_elo);
            break;
        case GLUT_KEY_UP:
            puzzle_mover_face_cima(&g_elo);
            break;
        case GLUT_KEY_DOWN:
            puzzle_mover_face_baixo(&g_elo);
            break;
    }
    glutPostRedisplay();
}

static void callback_mouse(int botao, int estado, int x, int y) {
    camera_mouse_click(&g_camera, botao, estado, x, y);
    glutPostRedisplay();
}

static void callback_motion(int x, int y) {
    camera_mouse_arrasto(&g_camera, x, y);
    glutPostRedisplay();
}

static void callback_timer(int valor) {
    (void)valor;
    const float deltaTempo = 0.016f; /* ~60 FPS */
    puzzle_atualizar_animacao(&g_elo, deltaTempo);

    glutPostRedisplay();
    glutTimerFunc(16, callback_timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_larguraJanela, g_alturaJanela);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Elo Maluco 3D - Computacao Grafica UNIFEI (ECOI24)");

    inicializar_opengl();
    camera_inicializar(&g_camera);
    puzzle_inicializar(&g_elo);

    /* Registro dos callbacks do FreeGLUT */
    glutDisplayFunc(callback_display);
    glutReshapeFunc(callback_reshape);
    glutKeyboardFunc(callback_keyboard);
    glutSpecialFunc(callback_special);
    glutMouseFunc(callback_mouse);
    glutMotionFunc(callback_motion);
    glutTimerFunc(16, callback_timer, 0);

    printf("========================================================\n");
    printf(" Elo Maluco 3D - Computacao Grafica (ECOI24 - UNIFEI)  \n");
    printf("========================================================\n");
    printf(" [D/A] Rotacionar anel superior (Direita / Esquerda)\n");
    printf(" [L/J] Rotacionar anel inferior (Direita / Esquerda)\n");
    printf(" [W/S] Mover face (Cima / Baixo)\n");
    printf(" [E] Embaralhar  |  [R] Resetar  |  [C] Resetar Camera\n");
    printf(" [Mouse Esquerdo + Arrastar] Rotacao orbital da camera\n");
    printf(" [Scroll / +/-] Zoom in / Zoom out\n");
    printf("========================================================\n");

    glutMainLoop();
    return 0;
}
