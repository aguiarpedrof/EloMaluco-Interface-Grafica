/**
 * @file render.c
 * @brief Implementação da modelagem geométrica 3D, materiais e renderização
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#include "render.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Configuração geométrica do modelo 3D */
static const float RAIO_CORE_INTERNO = 1.16f;
static const float ALTURA_TORRE_TOTAL = 3.60f;
static const int SEGMENTOS_CIRCULO = 48;
static const int SEGMENTOS_PECA = 12;

void render_inicializar(void) {
    /* Configuração de normais automáticas */
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

/* Desenha o cilindro central metálico onde as peças se movimentam */
void render_desenhar_torre_central(void) {
    float yMin = -ALTURA_TORRE_TOTAL * 0.5f;
    float yMax =  ALTURA_TORRE_TOTAL * 0.5f;

    /* Material metálico escuro para o núcleo */
    glColor3f(0.18f, 0.20f, 0.24f);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float angulo = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        float nx = sinf(angulo);
        float nz = cosf(angulo);
        float x = RAIO_CORE_INTERNO * nx;
        float z = RAIO_CORE_INTERNO * nz;

        glNormal3f(nx, 0.0f, nz);
        glVertex3f(x, yMax, z);
        glVertex3f(x, yMin, z);
    }
    glEnd();
}

/* Desenha anéis metálicos da base e do topo */
void render_desenhar_base_e_topo(void) {
    float raioBase = 1.55f;
    float alturaAnel = 0.22f;
    float yTopo = ALTURA_TORRE_TOTAL * 0.5f + 0.05f;
    float yBase = -ALTURA_TORRE_TOTAL * 0.5f - 0.05f;

    /* Cor metálica grafite com bordas douradas */
    glColor3f(0.28f, 0.30f, 0.35f);

    /* Anel Superior */
    glPushMatrix();
    glTranslatef(0.0f, yTopo, 0.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float ang = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        float nx = sinf(ang);
        float nz = cosf(ang);
        glNormal3f(nx, 0.0f, nz);
        glVertex3f(raioBase * nx, alturaAnel * 0.5f, raioBase * nz);
        glVertex3f(raioBase * nx, -alturaAnel * 0.5f, raioBase * nz);
    }
    glEnd();
    
    /* Tampa do topo */
    glColor3f(0.22f, 0.24f, 0.28f);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, alturaAnel * 0.5f, 0.0f);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float ang = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        glVertex3f(raioBase * sinf(ang), alturaAnel * 0.5f, raioBase * cosf(ang));
    }
    glEnd();
    glPopMatrix();

    /* Anel Inferior (Pedestal) */
    glColor3f(0.28f, 0.30f, 0.35f);
    glPushMatrix();
    glTranslatef(0.0f, yBase, 0.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float ang = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        float nx = sinf(ang);
        float nz = cosf(ang);
        glNormal3f(nx, 0.0f, nz);
        glVertex3f((raioBase + 0.15f) * nx, -alturaAnel * 0.5f, (raioBase + 0.15f) * nz);
        glVertex3f(raioBase * nx, alturaAnel * 0.5f, raioBase * nz);
    }
    glEnd();
    glPopMatrix();
}

/* Guias verticais que separam as 4 colunas mecânicas */
void render_desenhar_guia_trilhos(void) {
    float yMin = -ALTURA_TORRE_TOTAL * 0.5f;
    float yMax =  ALTURA_TORRE_TOTAL * 0.5f;
    float r = RAIO_CORE_INTERNO + 0.04f;

    glColor3f(0.12f, 0.13f, 0.15f); /* Friso escuro */

    for (int col = 0; col < COLUNAS; col++) {
        float anguloCentro = (float)col * 90.0f + 45.0f;
        float rad = anguloCentro * (float)M_PI / 180.0f;
        float nx = sinf(rad);
        float nz = cosf(rad);

        glLineWidth(2.5f);
        glBegin(GL_LINES);
            glVertex3f(r * nx, yMin, r * nz);
            glVertex3f(r * nx, yMax, r * nz);
        glEnd();
    }
}

/* Desenha uma peça individual com geometria tridimensional curvada */
void render_desenhar_peca_3d(const Peca* peca, int linha, int coluna, bool resolvido) {
    (void)linha; (void)coluna;

    /* Não renderiza nada na posição vazia para revelar o interior do puzzle */
    if (peca->cor == COR_VAZIO) {
        return;
    }

    /* Raio externo depende da altura da peça (Superior, Meio, Inferior) */
    float raioInterno = RAIO_CORE_INTERNO + 0.03f;
    float raioExterno = raioInterno + 0.22f; /* padrão 'm' */

    if (peca->altura == ALTURA_SUPERIOR) {
        raioExterno = raioInterno + 0.30f; /* Maior relevo */
    } else if (peca->altura == ALTURA_INFERIOR) {
        raioExterno = raioInterno + 0.14f; /* Menor relevo */
    }

    float meiaAltura = 0.35f;
    float anguloAbertura = 35.0f; /* Abertura angular em graus para cada lado (-35 a +35) */

    glPushMatrix();

    /* APLICAÇÃO DE TRANSFORMAÇÕES GEOMÉTRICAS (Requisito fundamental) */
    /* 1. ROTAÇÃO: oriento o anel da peça em torno do eixo Y */
    glRotatef(peca->rotY, 0.0f, 1.0f, 0.0f);

    /* 2. TRANSLAÇÃO: posiciono na altura Y da linha (com animação) */
    glTranslatef(0.0f, peca->posY, 0.0f);

    /* 3. ESCALA: efeito de pulsação suave em caso de vitória */
    if (resolvido) {
        glScalef(peca->escala, peca->escala, peca->escala);
    }

    /* Define cor da peça */
    glColor3f(peca->r, peca->g, peca->b);

    /* 1. Face Curvada Frontal (Externa) */
    glBegin(GL_QUAD_STRIP);
    for (int s = 0; s <= SEGMENTOS_PECA; s++) {
        float t = (float)s / (float)SEGMENTOS_PECA;
        float angDeg = -anguloAbertura + t * (2.0f * anguloAbertura);
        float rad = angDeg * (float)M_PI / 180.0f;
        float nx = sinf(rad);
        float nz = cosf(rad);

        glNormal3f(nx, 0.0f, nz);
        glVertex3f(raioExterno * nx,  meiaAltura, raioExterno * nz);
        glVertex3f(raioExterno * nx, -meiaAltura, raioExterno * nz);
    }
    glEnd();

    /* 2. Face Curvada Traseira (Interna) */
    glBegin(GL_QUAD_STRIP);
    for (int s = 0; s <= SEGMENTOS_PECA; s++) {
        float t = (float)s / (float)SEGMENTOS_PECA;
        float angDeg = anguloAbertura - t * (2.0f * anguloAbertura);
        float rad = angDeg * (float)M_PI / 180.0f;
        float nx = sinf(rad);
        float nz = cosf(rad);

        glNormal3f(-nx, 0.0f, -nz);
        glVertex3f(raioInterno * nx,  meiaAltura, raioInterno * nz);
        glVertex3f(raioInterno * nx, -meiaAltura, raioInterno * nz);
    }
    glEnd();

    /* 3. Face Superior da Peça */
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int s = 0; s <= SEGMENTOS_PECA; s++) {
        float t = (float)s / (float)SEGMENTOS_PECA;
        float angDeg = -anguloAbertura + t * (2.0f * anguloAbertura);
        float rad = angDeg * (float)M_PI / 180.0f;
        float nx = sinf(rad);
        float nz = cosf(rad);

        glVertex3f(raioExterno * nx, meiaAltura, raioExterno * nz);
        glVertex3f(raioInterno * nx, meiaAltura, raioInterno * nz);
    }
    glEnd();

    /* 4. Face Inferior da Peça */
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0.0f, -1.0f, 0.0f);
    for (int s = 0; s <= SEGMENTOS_PECA; s++) {
        float t = (float)s / (float)SEGMENTOS_PECA;
        float angDeg = anguloAbertura - t * (2.0f * anguloAbertura);
        float rad = angDeg * (float)M_PI / 180.0f;
        float nx = sinf(rad);
        float nz = cosf(rad);

        glVertex3f(raioExterno * nx, -meiaAltura, raioExterno * nz);
        glVertex3f(raioInterno * nx, -meiaAltura, raioInterno * nz);
    }
    glEnd();

    /* 5. Bordas Laterais Esquerda e Direita (fechamento do poliedro) */
    float radEsq = -anguloAbertura * (float)M_PI / 180.0f;
    float radDir =  anguloAbertura * (float)M_PI / 180.0f;

    /* Lateral Esquerda */
    glBegin(GL_QUADS);
        glNormal3f(-cosf(radEsq), 0.0f, sinf(radEsq));
        glVertex3f(raioInterno * sinf(radEsq), -meiaAltura, raioInterno * cosf(radEsq));
        glVertex3f(raioExterno * sinf(radEsq), -meiaAltura, raioExterno * cosf(radEsq));
        glVertex3f(raioExterno * sinf(radEsq),  meiaAltura, raioExterno * cosf(radEsq));
        glVertex3f(raioInterno * sinf(radEsq),  meiaAltura, raioInterno * cosf(radEsq));
    glEnd();

    /* Lateral Direita */
    glBegin(GL_QUADS);
        glNormal3f(cosf(radDir), 0.0f, -sinf(radDir));
        glVertex3f(raioInterno * sinf(radDir),  meiaAltura, raioInterno * cosf(radDir));
        glVertex3f(raioExterno * sinf(radDir),  meiaAltura, raioExterno * cosf(radDir));
        glVertex3f(raioExterno * sinf(radDir), -meiaAltura, raioExterno * cosf(radDir));
        glVertex3f(raioInterno * sinf(radDir), -meiaAltura, raioInterno * cosf(radDir));
    glEnd();

    /* Contorno chanfrado escuro sutil para realce das arestas */
    glColor3f(peca->r * 0.6f, peca->g * 0.6f, peca->b * 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (int s = 0; s <= SEGMENTOS_PECA; s++) {
        float t = (float)s / (float)SEGMENTOS_PECA;
        float angDeg = -anguloAbertura + t * (2.0f * anguloAbertura);
        float rad = angDeg * (float)M_PI / 180.0f;
        glVertex3f((raioExterno + 0.005f) * sinf(rad), meiaAltura, (raioExterno + 0.005f) * cosf(rad));
    }
    for (int s = SEGMENTOS_PECA; s >= 0; s--) {
        float t = (float)s / (float)SEGMENTOS_PECA;
        float angDeg = -anguloAbertura + t * (2.0f * anguloAbertura);
        float rad = angDeg * (float)M_PI / 180.0f;
        glVertex3f((raioExterno + 0.005f) * sinf(rad), -meiaAltura, (raioExterno + 0.005f) * cosf(rad));
    }
    glEnd();

    glPopMatrix();
}

/* Desenha a mesa de madeira onde o puzzle está apoiado */
void render_desenhar_mesa(void) {
    float yMesa = -ALTURA_TORRE_TOTAL * 0.5f - 0.28f;
    float tamanho = 12.0f;

    /* Mesa de madeira elegante */
    glColor3f(0.35f, 0.22f, 0.14f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-tamanho, yMesa, -tamanho);
        glVertex3f(-tamanho, yMesa,  tamanho);
        glVertex3f( tamanho, yMesa,  tamanho);
        glVertex3f( tamanho, yMesa, -tamanho);
    glEnd();
}

/* Renderização principal da cena 3D */
void render_desenhar_cena(const EloMaluco* elo, const Camera* cam, bool exibirHUD, int largura, int altura) {
    /* 1. Desenha a mesa de suporte */
    render_desenhar_mesa();

    /* 2. Desenha o núcleo central e os anéis estruturais */
    render_desenhar_torre_central();
    render_desenhar_base_e_topo();
    render_desenhar_guia_trilhos();

    /* 3. Desenha as 15 peças 3D do Elo Maluco */
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            render_desenhar_peca_3d(&elo->grade[i][j], i, j, elo->resolvido);
        }
    }

    /* 4. Desenha o HUD caso ativado */
    if (exibirHUD) {
        render_desenhar_hud(elo, cam, largura, altura);
    }
}

/* Desenha texto bitmap no HUD 2D */
static void desenhar_texto_2d(float x, float y, const char* texto) {
    glRasterPos2f(x, y);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *texto);
        texto++;
    }
}

/* HUD 2D em Projeção Ortográfica */
void render_desenhar_hud(const EloMaluco* elo, const Camera* cam, int largura, int altura) {
    (void)cam;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, largura, 0, altura);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    /* Fundo translúcido para o painel de controles */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.06f, 0.08f, 0.75f);
    glBegin(GL_QUADS);
        glVertex2f(10, altura - 10);
        glVertex2f(340, altura - 10);
        glVertex2f(340, altura - 230);
        glVertex2f(10, altura - 230);
    glEnd();
    glDisable(GL_BLEND);

    /* Moldura do painel */
    glColor3f(0.3f, 0.4f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(10, altura - 10);
        glVertex2f(340, altura - 10);
        glVertex2f(340, altura - 230);
        glVertex2f(10, altura - 230);
    glEnd();

    /* Textos informativos */
    glColor3f(1.0f, 0.85f, 0.2f);
    desenhar_texto_2d(20, altura - 30, "ELO MALUCO 3D - COMPUTACAO GRAFICA (UNIFEI)");

    char buffer[128];
    if (elo->resolvido) {
        glColor3f(0.2f, 0.95f, 0.3f);
        desenhar_texto_2d(20, altura - 52, "STATUS: RESOLVIDO! PARABENS!");
    } else {
        glColor3f(0.95f, 0.5f, 0.2f);
        desenhar_texto_2d(20, altura - 52, "STATUS: EM JOGO (EMBARALHADO)");
    }

    glColor3f(0.9f, 0.9f, 0.9f);
    snprintf(buffer, sizeof(buffer), "Movimentos realizados: %d", elo->totalMovimentos);
    desenhar_texto_2d(20, altura - 72, buffer);

    glColor3f(0.7f, 0.8f, 0.9f);
    desenhar_texto_2d(20, altura - 100, "[D / A] Girar linha Superior (Dir / Esq)");
    desenhar_texto_2d(20, altura - 118, "[L / J] Girar linha Inferior (Dir / Esq)");
    desenhar_texto_2d(20, altura - 136, "[W / S] Mover face (Cima / Baixo)");
    desenhar_texto_2d(20, altura - 154, "[E] Embaralhar  |  [R] Resetar");
    desenhar_texto_2d(20, altura - 172, "[Mouse Esq + Arrastar] Rotacao da Camera");
    desenhar_texto_2d(20, altura - 190, "[Scroll / +/-] Zoom da Camera");
    desenhar_texto_2d(20, altura - 208, "[H] Ocultar/Exibir este painel");

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
