/**
 * @file render.c
 * @brief Implementação da modelagem geométrica 3D, materiais e renderização
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#include "render.h"
#include "texture.h"
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
    /* Configuração de normais automáticas e sombreamento Gouraud/Phong suave */
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    /* Habilita o pipeline de iluminação */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    /* Luz Principal (Key Light) - Diagonal superior frontal */
    GLfloat luz0Pos[]      = { 5.0f, 9.0f, 7.0f, 1.0f };
    GLfloat luz0Ambiente[] = { 0.25f, 0.25f, 0.28f, 1.0f };
    GLfloat luz0Difusa[]   = { 0.85f, 0.85f, 0.85f, 1.0f };
    GLfloat luz0Especular[]= { 0.95f, 0.95f, 0.95f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, luz0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz0Ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0Difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0Especular);

    /* Luz de Preenchimento (Fill Light) - Oposta para suavizar sombras */
    GLfloat luz1Pos[]      = { -6.0f, 3.0f, -6.0f, 1.0f };
    GLfloat luz1Ambiente[] = { 0.05f, 0.05f, 0.08f, 1.0f };
    GLfloat luz1Difusa[]   = { 0.35f, 0.35f, 0.40f, 1.0f };
    GLfloat luz1Especular[]= { 0.25f, 0.25f, 0.25f, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, luz1Pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, luz1Ambiente);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luz1Difusa);
    glLightfv(GL_LIGHT1, GL_SPECULAR, luz1Especular);

    /* Iluminação global ambiente */
    GLfloat luzGlobalAmbiente[] = { 0.20f, 0.20f, 0.22f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzGlobalAmbiente);

    /* Rastreamento de cores para materiais: glColor define ambiente e difusa */
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    /* Brilho especular das peças plásticas (efeito brilhante/lustroso) */
    GLfloat matEspecular[] = { 0.80f, 0.80f, 0.80f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matEspecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);

    /* Inicialização do sistema de texturas */
    texture_inicializar();
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

/* Desenha anéis metálicos da base e do topo com textura de metal */
void render_desenhar_base_e_topo(void) {
    float raioBase = 1.55f;
    float alturaAnel = 0.22f;
    float yTopo = ALTURA_TORRE_TOTAL * 0.5f + 0.05f;
    float yBase = -ALTURA_TORRE_TOTAL * 0.5f - 0.05f;

    GLuint texMetal = texture_obter_id(TEXTURA_METAL_ANEL);
    if (texMetal > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texMetal);
        glColor3f(0.85f, 0.85f, 0.90f);
    } else {
        glColor3f(0.28f, 0.30f, 0.35f);
    }

    /* Anel Superior */
    glPushMatrix();
    glTranslatef(0.0f, yTopo, 0.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float u = (float)i / (float)SEGMENTOS_CIRCULO;
        float ang = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        float nx = sinf(ang);
        float nz = cosf(ang);
        glNormal3f(nx, 0.0f, nz);
        glTexCoord2f(u * 2.0f, 1.0f); glVertex3f(raioBase * nx, alturaAnel * 0.5f, raioBase * nz);
        glTexCoord2f(u * 2.0f, 0.0f); glVertex3f(raioBase * nx, -alturaAnel * 0.5f, raioBase * nz);
    }
    glEnd();
    
    /* Tampa do topo */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, alturaAnel * 0.5f, 0.0f);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float ang = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        float nx = sinf(ang);
        float nz = cosf(ang);
        glTexCoord2f(0.5f + 0.5f * nx, 0.5f + 0.5f * nz);
        glVertex3f(raioBase * nx, alturaAnel * 0.5f, raioBase * nz);
    }
    glEnd();
    glPopMatrix();

    /* Anel Inferior (Pedestal) */
    glPushMatrix();
    glTranslatef(0.0f, yBase, 0.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTOS_CIRCULO; i++) {
        float u = (float)i / (float)SEGMENTOS_CIRCULO;
        float ang = (float)i * 2.0f * (float)M_PI / (float)SEGMENTOS_CIRCULO;
        float nx = sinf(ang);
        float nz = cosf(ang);
        glNormal3f(nx, 0.0f, nz);
        glTexCoord2f(u * 2.0f, 0.0f); glVertex3f((raioBase + 0.15f) * nx, -alturaAnel * 0.5f, (raioBase + 0.15f) * nz);
        glTexCoord2f(u * 2.0f, 1.0f); glVertex3f(raioBase * nx, alturaAnel * 0.5f, raioBase * nz);
    }
    glEnd();
    glPopMatrix();

    if (texMetal > 0) {
        glDisable(GL_TEXTURE_2D);
    }
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
    GLuint texMadeira = texture_obter_id(TEXTURA_MESA_MADEIRA);

    if (texMadeira > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texMadeira);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glColor3f(0.35f, 0.22f, 0.14f);
    }

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-tamanho, yMesa, -tamanho);
        glTexCoord2f(0.0f, 6.0f); glVertex3f(-tamanho, yMesa,  tamanho);
        glTexCoord2f(6.0f, 6.0f); glVertex3f( tamanho, yMesa,  tamanho);
        glTexCoord2f(6.0f, 0.0f); glVertex3f( tamanho, yMesa, -tamanho);
    glEnd();

    if (texMadeira > 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

/* Renderização principal da cena 3D */
void render_desenhar_cena(const EloMaluco* elo, const Camera* cam, bool exibirHUD, int largura, int altura) {
    /* 1. Desenha a mesa de suporte (estática) */
    render_desenhar_mesa();

    glPushMatrix();
    /* Rotação Contínua do Puzzle (Demonstração de Rotação 3D - Turntable) */
    if (elo->modoTurntable) {
        glRotatef(elo->anguloTurntable, 0.0f, 1.0f, 0.0f);
    }

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
    glPopMatrix();

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
    glDisable(GL_LIGHTING);

    /* Fundo translúcido para o painel de controles */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.06f, 0.08f, 0.80f);
    glBegin(GL_QUADS);
        glVertex2f(10, altura - 10);
        glVertex2f(370, altura - 10);
        glVertex2f(370, altura - 300);
        glVertex2f(10, altura - 300);
    glEnd();
    glDisable(GL_BLEND);

    /* Moldura do painel */
    glColor3f(0.35f, 0.45f, 0.55f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(10, altura - 10);
        glVertex2f(370, altura - 10);
        glVertex2f(370, altura - 300);
        glVertex2f(10, altura - 300);
    glEnd();

    /* Textos informativos */
    glColor3f(1.0f, 0.85f, 0.2f);
    desenhar_texto_2d(20, altura - 30, "ELO MALUCO 3D - COMPUTACAO GRAFICA (UNIFEI)");

    char buffer[128];
    if (elo->resolvido) {
        glColor3f(0.2f, 0.95f, 0.3f);
        desenhar_texto_2d(20, altura - 50, "STATUS: RESOLVIDO! (PARABENS)");
    } else if (elo->modoAutoPlay) {
        glColor3f(0.3f, 0.7f, 1.0f);
        snprintf(buffer, sizeof(buffer), "STATUS: EXECUTANDO SOLUCAO (%d/%d)", elo->indiceFila, elo->totalFila);
        desenhar_texto_2d(20, altura - 50, buffer);
    } else {
        glColor3f(0.95f, 0.5f, 0.2f);
        desenhar_texto_2d(20, altura - 50, "STATUS: EM JOGO (EMBARALHADO)");
    }

    glColor3f(0.9f, 0.9f, 0.9f);
    snprintf(buffer, sizeof(buffer), "Movimentos: %d  |  Turntable: %s", 
             elo->totalMovimentos, elo->modoTurntable ? "LIGADO (Rotacao 3D)" : "DESLIGADO");
    desenhar_texto_2d(20, altura - 70, buffer);

    /* Lista detalhada de comandos */
    glColor3f(0.75f, 0.85f, 0.95f);
    desenhar_texto_2d(20, altura - 95,  "[D / A] Girar linha Superior (Dir / Esq)");
    desenhar_texto_2d(20, altura - 113, "[L / J] Girar linha Inferior (Dir / Esq)");
    desenhar_texto_2d(20, altura - 131, "[W / S] Mover face (Cima / Baixo)");
    desenhar_texto_2d(20, altura - 149, "[E] Embaralhar  |  [R] Resetar");
    desenhar_texto_2d(20, altura - 167, "[Espaco] Solucao Demonstrativa (Auto-play)");
    desenhar_texto_2d(20, altura - 185, "[T] Alternar Modo Turntable (Rotacao)");
    desenhar_texto_2d(20, altura - 203, "[P / Tab] Selecionar e Pulsar Peca (Escala)");
    desenhar_texto_2d(20, altura - 221, "[Mouse Esq + Arrastar] Rotacao da Camera");
    desenhar_texto_2d(20, altura - 239, "[Scroll / +/-] Zoom / Escala da Camera");
    desenhar_texto_2d(20, altura - 257, "[C] Resetar Camera  |  [H] Alternar HUD");

    glColor3f(0.6f, 0.65f, 0.7f);
    desenhar_texto_2d(20, altura - 285, "Transformacoes: Translacao, Rotacao e Escala");

    /* Painel do Mini-mapa 4x4 (Visão Desdobrada / Planificada das 4 Faces) */
    float mapaX = (float)largura - 195.0f;
    float mapaY = (float)altura - 10.0f;
    float mapaLargura = 185.0f;
    float mapaAltura = 195.0f;

    /* Fundo do mini-mapa */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.06f, 0.08f, 0.80f);
    glBegin(GL_QUADS);
        glVertex2f(mapaX, mapaY);
        glVertex2f(mapaX + mapaLargura, mapaY);
        glVertex2f(mapaX + mapaLargura, mapaY - mapaAltura);
        glVertex2f(mapaX, mapaY - mapaAltura);
    glEnd();
    glDisable(GL_BLEND);

    /* Moldura */
    glColor3f(0.35f, 0.45f, 0.55f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(mapaX, mapaY);
        glVertex2f(mapaX + mapaLargura, mapaY);
        glVertex2f(mapaX + mapaLargura, mapaY - mapaAltura);
        glVertex2f(mapaX, mapaY - mapaAltura);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.2f);
    desenhar_texto_2d(mapaX + 18.0f, mapaY - 24.0f, "MAPA 4x4 DAS FACES");

    /* Desenha os 16 blocos da grade */
    float celulaW = 34.0f;
    float celulaH = 28.0f;
    float gridStartX = mapaX + 18.0f;
    float gridStartY = mapaY - 38.0f;

    for (int l = 0; l < LINHAS; l++) {
        for (int c = 0; c < COLUNAS; c++) {
            float x0 = gridStartX + (float)c * (celulaW + 4.0f);
            float y0 = gridStartY - (float)l * (celulaH + 4.0f);
            float x1 = x0 + celulaW;
            float y1 = y0 - celulaH;

            const Peca* p = &elo->grade[l][c];
            if (p->cor == COR_VAZIO) {
                /* Espaço vazio: cinza escuro com cruz indicativa */
                glColor3f(0.12f, 0.13f, 0.18f);
                glBegin(GL_QUADS);
                    glVertex2f(x0, y0);
                    glVertex2f(x1, y0);
                    glVertex2f(x1, y1);
                    glVertex2f(x0, y1);
                glEnd();
                glColor3f(0.45f, 0.5f, 0.6f);
                glBegin(GL_LINES);
                    glVertex2f(x0, y0); glVertex2f(x1, y1);
                    glVertex2f(x0, y1); glVertex2f(x1, y0);
                glEnd();
            } else {
                /* Peça colorida */
                glColor3f(p->r, p->g, p->b);
                glBegin(GL_QUADS);
                    glVertex2f(x0, y0);
                    glVertex2f(x1, y0);
                    glVertex2f(x1, y1);
                    glVertex2f(x0, y1);
                glEnd();

                /* Indicador de altura: 'S', 'M', 'I' */
                char letraH = (p->altura == ALTURA_SUPERIOR) ? 'S' : 
                              (p->altura == ALTURA_INFERIOR) ? 'I' : 'M';
                char strH[2] = { letraH, '\0' };
                if (p->cor == COR_BRANCO || p->cor == COR_AMARELO) {
                    glColor3f(0.1f, 0.1f, 0.1f);
                } else {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
                desenhar_texto_2d(x0 + 12.0f, y1 + 9.0f, strH);
            }

            /* Borda da célula com destaque para peça selecionada */
            if (l == elo->linhaSelecionada && c == elo->colunaSelecionada) {
                glColor3f(1.0f, 0.95f, 0.2f);
                glLineWidth(2.2f);
            } else {
                glColor3f(0.25f, 0.3f, 0.38f);
                glLineWidth(1.0f);
            }
            glBegin(GL_LINE_LOOP);
                glVertex2f(x0, y0);
                glVertex2f(x1, y0);
                glVertex2f(x1, y1);
                glVertex2f(x0, y1);
            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
