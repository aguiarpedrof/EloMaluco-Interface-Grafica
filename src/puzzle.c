/**
 * @file puzzle.c
 * @brief Implementação da lógica, regras e transformações do Elo Maluco
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#include "puzzle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Configuração geométrica do cilindro da torre */
static const float RAIO_TORRE = 1.35f;
static const float ALTURAS_Y[LINHAS] = { 1.20f, 0.40f, -0.40f, -1.20f };
static const float ANGULOS_COLUNA[COLUNAS] = { 0.0f, 90.0f, 180.0f, 270.0f };

/* Tabela de cores RGB base */
static void configurar_cor_e_altura(Peca* p) {
    if (strcmp(p->codigo, "vzo") == 0 || strcmp(p->codigo, "vazio") == 0) {
        p->cor = COR_VAZIO;
        p->altura = ALTURA_VAZIA;
        p->r = 0.15f; p->g = 0.15f; p->b = 0.18f;
        return;
    }
    
    /* Decodifica cor pelos 2 primeiros caracteres */
    if (p->codigo[0] == 'v' && p->codigo[1] == 'm') {
        p->cor = COR_VERDE;
        p->r = 0.18f; p->g = 0.82f; p->b = 0.28f; /* Verde vibrante */
    } else if (p->codigo[0] == 'v' && p->codigo[1] == 'r') {
        p->cor = COR_VERMELHO;
        p->r = 0.92f; p->g = 0.18f; p->b = 0.18f; /* Vermelho vivo */
    } else if (p->codigo[0] == 'a' && p->codigo[1] == 'm') {
        p->cor = COR_AMARELO;
        p->r = 0.98f; p->g = 0.84f; p->b = 0.14f; /* Amarelo dourado */
    } else if (p->codigo[0] == 'b' && p->codigo[1] == 'r') {
        p->cor = COR_BRANCO;
        p->r = 0.95f; p->g = 0.95f; p->b = 0.98f; /* Branco perolado */
    } else {
        p->cor = COR_VAZIO;
        p->r = 0.5f; p->g = 0.5f; p->b = 0.5f;
    }

    /* Decodifica altura pelo 3º caractere */
    char h = p->codigo[2];
    if (h == 's') p->altura = ALTURA_SUPERIOR;
    else if (h == 'm') p->altura = ALTURA_MEIO_1;
    else if (h == 'i') p->altura = ALTURA_INFERIOR;
    else p->altura = ALTURA_MEIO_1;
}

static void calcular_coordenadas_slot(int linha, int coluna, float* x, float* y, float* z, float* anguloY) {
    *y = ALTURAS_Y[linha];
    *anguloY = ANGULOS_COLUNA[coluna];
    float rad = (*anguloY) * (float)M_PI / 180.0f;
    *x = RAIO_TORRE * sinf(rad);
    *z = RAIO_TORRE * cosf(rad);
}

static void atualizar_targets_instantaneo(EloMaluco* elo) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            Peca* p = &elo->grade[i][j];
            float tx, ty, tz, trot;
            calcular_coordenadas_slot(i, j, &tx, &ty, &tz, &trot);
            p->targetX = tx; p->posX = tx;
            p->targetY = ty; p->posY = ty;
            p->targetZ = tz; p->posZ = tz;
            p->targetRotY = trot; p->rotY = trot;
            p->escala = 1.0f;
            p->emMovimento = false;
        }
    }
}

void puzzle_inicializar(EloMaluco* elo) {
    static const char* estadoResolvido[LINHAS][COLUNAS] = {
        { "vms", "vrs", "ams", "brs" },
        { "vmm", "vrm", "amm", "brm" },
        { "vmm", "vrm", "amm", "vzo" },
        { "vmi", "vri", "ami", "bri" }
    };

    memset(elo, 0, sizeof(EloMaluco));
    elo->totalMovimentos = 0;
    elo->resolvido = true;
    elo->animando = false;
    elo->progressoAnimacao = 1.0f;
    elo->vazioLinha = 2;
    elo->vazioColuna = 3;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            Peca* p = &elo->grade[i][j];
            strncpy(p->codigo, estadoResolvido[i][j], 3);
            p->codigo[3] = '\0';
            configurar_cor_e_altura(p);
        }
    }

    atualizar_targets_instantaneo(elo);
    srand((unsigned int)time(NULL));
}

void puzzle_resetar(EloMaluco* elo) {
    puzzle_inicializar(elo);
}

/* Verifica se o estado atual é o estado objetivo resolvido */
bool puzzle_eh_estado_objetivo(const EloMaluco* elo) {
    int colBranco = -1;

    /* Cada coluna deve ter uma cor uniforme (exceto vzo) */
    for (int c = 0; c < COLUNAS; c++) {
        char corBase[3] = { elo->grade[0][c].codigo[0], elo->grade[0][c].codigo[1], '\0' };
        if (corBase[0] == 'v' && corBase[1] == 'z') return false;
        if (corBase[0] == 'b' && corBase[1] == 'r') colBranco = c;

        for (int l = 1; l < LINHAS; l++) {
            const char* cod = elo->grade[l][c].codigo;
            if (strcmp(cod, "vzo") != 0 && (cod[0] != corBase[0] || cod[1] != corBase[1])) {
                return false;
            }
        }
    }

    if (colBranco == -1) return false;

    /* Verifica padrão de alturas: Superior, Meio, Meio, Inferior */
    for (int c = 0; c < COLUNAS; c++) {
        char alturas[LINHAS];
        int count = 0;
        for (int l = 0; l < LINHAS; l++) {
            if (strcmp(elo->grade[l][c].codigo, "vzo") != 0) {
                alturas[count++] = elo->grade[l][c].codigo[2];
            }
        }

        if (c == colBranco) {
            if (count != 3) return false;
            if (alturas[0] != 's' || alturas[1] != 'm' || alturas[2] != 'i') return false;
        } else {
            if (count != 4) return false;
            if (alturas[0] != 's' || alturas[1] != 'm' || alturas[2] != 'm' || alturas[3] != 'i') return false;
        }
    }

    return true;
}

/* Prepara targets para animação suave após movimentação */
static void engatilhar_animacao_targets(EloMaluco* elo) {
    elo->animando = true;
    elo->progressoAnimacao = 0.0f;
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            Peca* p = &elo->grade[i][j];
            float tx, ty, tz, trot;
            calcular_coordenadas_slot(i, j, &tx, &ty, &tz, &trot);
            p->targetX = tx;
            p->targetY = ty;
            p->targetZ = tz;
            p->targetRotY = trot;
            p->emMovimento = true;
        }
    }
}

/* rsd - Rotacionar Linha Superior para Direita (col 0->1->2->3->0) */
bool puzzle_rotacionar_superior_direita(EloMaluco* elo) {
    Peca temp = elo->grade[0][COLUNAS - 1];
    for (int c = COLUNAS - 1; c > 0; c--) {
        elo->grade[0][c] = elo->grade[0][c - 1];
    }
    elo->grade[0][0] = temp;

    if (elo->vazioLinha == 0) {
        elo->vazioColuna = (elo->vazioColuna + 1) % COLUNAS;
    }

    elo->totalMovimentos++;
    engatilhar_animacao_targets(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
    return true;
}

/* rse - Rotacionar Linha Superior para Esquerda (col 3->2->1->0->3) */
bool puzzle_rotacionar_superior_esquerda(EloMaluco* elo) {
    Peca temp = elo->grade[0][0];
    for (int c = 0; c < COLUNAS - 1; c++) {
        elo->grade[0][c] = elo->grade[0][c + 1];
    }
    elo->grade[0][COLUNAS - 1] = temp;

    if (elo->vazioLinha == 0) {
        elo->vazioColuna = (elo->vazioColuna - 1 + COLUNAS) % COLUNAS;
    }

    elo->totalMovimentos++;
    engatilhar_animacao_targets(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
    return true;
}

/* rid - Rotacionar Linha Inferior para Direita */
bool puzzle_rotacionar_inferior_direita(EloMaluco* elo) {
    Peca temp = elo->grade[LINHAS - 1][COLUNAS - 1];
    for (int c = COLUNAS - 1; c > 0; c--) {
        elo->grade[LINHAS - 1][c] = elo->grade[LINHAS - 1][c - 1];
    }
    elo->grade[LINHAS - 1][0] = temp;

    if (elo->vazioLinha == LINHAS - 1) {
        elo->vazioColuna = (elo->vazioColuna + 1) % COLUNAS;
    }

    elo->totalMovimentos++;
    engatilhar_animacao_targets(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
    return true;
}

/* rie - Rotacionar Linha Inferior para Esquerda */
bool puzzle_rotacionar_inferior_esquerda(EloMaluco* elo) {
    Peca temp = elo->grade[LINHAS - 1][0];
    for (int c = 0; c < COLUNAS - 1; c++) {
        elo->grade[LINHAS - 1][c] = elo->grade[LINHAS - 1][c + 1];
    }
    elo->grade[LINHAS - 1][COLUNAS - 1] = temp;

    if (elo->vazioLinha == LINHAS - 1) {
        elo->vazioColuna = (elo->vazioColuna - 1 + COLUNAS) % COLUNAS;
    }

    elo->totalMovimentos++;
    engatilhar_animacao_targets(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
    return true;
}

/* mfc - Mover Face Cima (peça abaixo do vazio sobe, vazio desce) */
bool puzzle_mover_face_cima(EloMaluco* elo) {
    int vl = elo->vazioLinha;
    int vc = elo->vazioColuna;
    if (vl + 1 >= LINHAS) return false; /* Não há peça abaixo */

    Peca temp = elo->grade[vl][vc];
    elo->grade[vl][vc] = elo->grade[vl + 1][vc];
    elo->grade[vl + 1][vc] = temp;
    elo->vazioLinha = vl + 1;

    elo->totalMovimentos++;
    engatilhar_animacao_targets(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
    return true;
}

/* mfb - Mover Face Baixo (peça acima do vazio desce, vazio sobe) */
bool puzzle_mover_face_baixo(EloMaluco* elo) {
    int vl = elo->vazioLinha;
    int vc = elo->vazioColuna;
    if (vl - 1 < 0) return false; /* Não há peça acima */

    Peca temp = elo->grade[vl][vc];
    elo->grade[vl][vc] = elo->grade[vl - 1][vc];
    elo->grade[vl - 1][vc] = temp;
    elo->vazioLinha = vl - 1;

    elo->totalMovimentos++;
    engatilhar_animacao_targets(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
    return true;
}

bool puzzle_executar_movimento(EloMaluco* elo, const char* acao) {
    bool ok = false;
    if (strcmp(acao, "rsd") == 0) ok = puzzle_rotacionar_superior_direita(elo);
    else if (strcmp(acao, "rse") == 0) ok = puzzle_rotacionar_superior_esquerda(elo);
    else if (strcmp(acao, "rid") == 0) ok = puzzle_rotacionar_inferior_direita(elo);
    else if (strcmp(acao, "rie") == 0) ok = puzzle_rotacionar_inferior_esquerda(elo);
    else if (strcmp(acao, "mfc") == 0) ok = puzzle_mover_face_cima(elo);
    else if (strcmp(acao, "mfb") == 0) ok = puzzle_mover_face_baixo(elo);

    if (ok && !elo->modoAutoPlay && elo->totalHistorico < 256) {
        strncpy(elo->historicoMovimentos[elo->totalHistorico], acao, 3);
        elo->historicoMovimentos[elo->totalHistorico][3] = '\0';
        elo->totalHistorico++;
    }

    return ok;
}

void puzzle_embaralhar(EloMaluco* elo, int numMovimentos) {
    const char* movimentosPossiveis[] = { "rsd", "rse", "rid", "rie", "mfc", "mfb" };
    int total = sizeof(movimentosPossiveis) / sizeof(movimentosPossiveis[0]);

    for (int i = 0; i < numMovimentos; i++) {
        int idx = rand() % total;
        puzzle_executar_movimento(elo, movimentosPossiveis[idx]);
    }
    atualizar_targets_instantaneo(elo);
    elo->resolvido = puzzle_eh_estado_objetivo(elo);
}

/* Embaralha o puzzle de forma sequencial e animada */
void puzzle_embaralhar_animado(EloMaluco* elo, int numMovimentos) {
    const char* movimentosPossiveis[] = { "rsd", "rse", "rid", "rie", "mfc", "mfb" };
    int total = sizeof(movimentosPossiveis) / sizeof(movimentosPossiveis[0]);

    if (numMovimentos > 256) numMovimentos = 256;
    elo->totalFila = 0;
    elo->totalHistorico = 0;

    for (int i = 0; i < numMovimentos; i++) {
        int idx = rand() % total;
        memcpy(elo->filaMovimentos[elo->totalFila], movimentosPossiveis[idx], 4);

        /* Grava para a solução reversa conseguir solucionar */
        memcpy(elo->historicoMovimentos[elo->totalHistorico], movimentosPossiveis[idx], 4);

        elo->totalFila++;
        elo->totalHistorico++;
    }

    elo->indiceFila = 0;
    elo->modoAutoPlay = true;
    elo->timerPassoAutoPlay = 0.0f;
}

static const char* obter_movimento_inverso(const char* acao) {
    if (strcmp(acao, "rsd") == 0) return "rse";
    if (strcmp(acao, "rse") == 0) return "rsd";
    if (strcmp(acao, "rid") == 0) return "rie";
    if (strcmp(acao, "rie") == 0) return "rid";
    if (strcmp(acao, "mfc") == 0) return "mfb";
    if (strcmp(acao, "mfb") == 0) return "mfc";
    return acao;
}

/* Sequência de solução demonstrativa inteligente */
void puzzle_iniciar_solucao_demo(EloMaluco* elo) {
    if (elo->totalHistorico > 0) {
        /* Constrói a solução inversa revertendo passo a passo */
        elo->totalFila = 0;
        for (int i = elo->totalHistorico - 1; i >= 0; i--) {
            const char* inv = obter_movimento_inverso(elo->historicoMovimentos[i]);
            memcpy(elo->filaMovimentos[elo->totalFila], inv, 4);
            elo->totalFila++;
        }
        elo->totalHistorico = 0;
        elo->indiceFila = 0;
        elo->modoAutoPlay = true;
        elo->timerPassoAutoPlay = 0.0f;
    } else {
        /* Se não há histórico (estava resolvido), primeiro demonstra embaralhar e depois resolver */
        static const char* sequenciaDemo[] = {
            "mfc", "rsd", "mfb", "rie", "mfc", "rse", "mfb", "rid",
            "rie", "mfb", "rsd", "mfc", "rid", "mfb", "rse", "mfc"
        };
        int count = sizeof(sequenciaDemo) / sizeof(sequenciaDemo[0]);

        elo->totalFila = 0;
        for (int i = 0; i < count && i < 256; i++) {
            memcpy(elo->filaMovimentos[i], sequenciaDemo[i], 4);
            elo->totalFila++;
        }
        elo->indiceFila = 0;
        elo->modoAutoPlay = true;
        elo->timerPassoAutoPlay = 0.0f;
    }
}

void puzzle_proximo_passo_demo(EloMaluco* elo) {
    if (!elo->modoAutoPlay || elo->indiceFila >= elo->totalFila) {
        elo->modoAutoPlay = false;
        return;
    }
    puzzle_executar_movimento(elo, elo->filaMovimentos[elo->indiceFila]);
    elo->indiceFila++;
    if (elo->indiceFila >= elo->totalFila) {
        elo->modoAutoPlay = false;
    }
}

void puzzle_alternar_turntable(EloMaluco* elo) {
    elo->modoTurntable = !elo->modoTurntable;
}

void puzzle_selecionar_proxima_peca(EloMaluco* elo) {
    elo->colunaSelecionada++;
    if (elo->colunaSelecionada >= COLUNAS) {
        elo->colunaSelecionada = 0;
        elo->linhaSelecionada = (elo->linhaSelecionada + 1) % LINHAS;
    }
}

/* Interpolação Linear (LERP) para animações suaves (~60 FPS) */
void puzzle_atualizar_animacao(EloMaluco* elo, float deltaTempo) {
    const float taxa = 12.0f * deltaTempo; /* Velocidade do LERP */
    bool aindaMovendo = false;

    elo->tempoTotal += deltaTempo;

    /* Atualiza rotação do modo Turntable contínuo */
    if (elo->modoTurntable) {
        elo->anguloTurntable += 30.0f * deltaTempo;
        if (elo->anguloTurntable >= 360.0f) {
            elo->anguloTurntable -= 360.0f;
        }
    }

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            Peca* p = &elo->grade[i][j];

            /* Interpolação espacial X, Y, Z (Translação geométrica) */
            float dx = p->targetX - p->posX;
            float dy = p->targetY - p->posY;
            float dz = p->targetZ - p->posZ;

            if (fabsf(dx) > 0.002f || fabsf(dy) > 0.002f || fabsf(dz) > 0.002f) {
                p->posX += dx * fminf(taxa, 1.0f);
                p->posY += dy * fminf(taxa, 1.0f);
                p->posZ += dz * fminf(taxa, 1.0f);
                aindaMovendo = true;
            } else {
                p->posX = p->targetX;
                p->posY = p->targetY;
                p->posZ = p->targetZ;
            }

            /* Interpolação angular RotY (Rotação geométrica) */
            float drot = p->targetRotY - p->rotY;
            if (fabsf(drot) > 0.1f) {
                p->rotY += drot * fminf(taxa, 1.0f);
                aindaMovendo = true;
            } else {
                p->rotY = p->targetRotY;
            }

            /* Atualização do Fator de Escala (Transformação de Escala) */
            if (elo->resolvido) {
                /* Efeito de pulsação festiva/vitória com defasagem espacial */
                float fase = elo->tempoTotal * 5.0f + (float)i * 0.8f + (float)j * 0.4f;
                p->escala = 1.0f + 0.08f * sinf(fase);
            } else if (i == elo->linhaSelecionada && j == elo->colunaSelecionada) {
                /* Destaque interativo por escala da peça selecionada */
                p->escala = 1.15f + 0.04f * sinf(elo->tempoTotal * 8.0f);
            } else {
                p->escala = 1.0f;
            }
        }
    }

    elo->animando = aindaMovendo;

    /* Avanço automático dos passos no modo AutoPlay / Demonstração */
    if (elo->modoAutoPlay) {
        elo->timerPassoAutoPlay += deltaTempo;
        if (elo->timerPassoAutoPlay >= 0.40f && !aindaMovendo) {
            elo->timerPassoAutoPlay = 0.0f;
            puzzle_proximo_passo_demo(elo);
        }
    }
}
