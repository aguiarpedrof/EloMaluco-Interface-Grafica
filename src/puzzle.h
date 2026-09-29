/**
 * @file puzzle.h
 * @brief Modelo de dados e lógica do quebra-cabeça Elo Maluco (4x4)
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdbool.h>

#define LINHAS 4
#define COLUNAS 4

/* Identificadores de Cores */
typedef enum {
    COR_VERDE = 0,    /* vm */
    COR_VERMELHO = 1, /* vr */
    COR_AMARELO = 2,  /* am */
    COR_BRANCO = 3,   /* br */
    COR_VAZIO = 4     /* vzo */
} CorPeca;

/* Identificadores de Altura (espessura/relevo geométrico) */
typedef enum {
    ALTURA_SUPERIOR = 0, /* 's' */
    ALTURA_MEIO_1 = 1,   /* 'm' */
    ALTURA_MEIO_2 = 2,   /* 'm' */
    ALTURA_INFERIOR = 3, /* 'i' */
    ALTURA_VAZIA = 4     /* espaço vazio */
} AlturaPeca;

/* Estrutura de uma Peça individual */
typedef struct {
    char codigo[4];        /* ex: "vms", "vrm", "vzo" */
    CorPeca cor;
    AlturaPeca altura;
    float r, g, b;         /* Cor RGB da peça */
    
    /* Coordenadas e Transformações Geométricas */
    float posX, posY, posZ;       /* Posição atual (interpolada na animação) */
    float targetX, targetY, targetZ; /* Posição alvo */
    float rotY;                   /* Rotação atual em graus */
    float targetRotY;             /* Rotação alvo em graus */
    float escala;                 /* Fator de escala (para efeito de pulsar/vitória) */
    
    bool emMovimento;             /* Flag indicando se a peça está animando */
} Peca;

/* Estrutura principal do Elo Maluco */
typedef struct {
    Peca grade[LINHAS][COLUNAS];
    int vazioLinha;
    int vazioColuna;
    
    /* Estatísticas e Estado */
    int totalMovimentos;
    bool resolvido;
    bool animando;
    float progressoAnimacao;      /* 0.0f a 1.0f */
    float tempoTotal;             /* Tempo acumulado em segundos */
    
    /* Demonstração de Transformação de Rotação Contínua (Turntable) */
    bool modoTurntable;
    float anguloTurntable;

    /* Demonstração de Transformação de Escala Interativa */
    int linhaSelecionada;
    int colunaSelecionada;
    
    /* Fila de movimentos automáticos (para demonstração/solução/embaralhamento) */
    char filaMovimentos[256][4];
    int totalFila;
    int indiceFila;
    bool modoAutoPlay;
    float timerPassoAutoPlay;
} EloMaluco;

/* Funções de Inicialização e Manipulação do Jogo */
void puzzle_inicializar(EloMaluco* elo);
void puzzle_resetar(EloMaluco* elo);
bool puzzle_eh_estado_objetivo(const EloMaluco* elo);
void puzzle_alternar_turntable(EloMaluco* elo);
void puzzle_selecionar_proxima_peca(EloMaluco* elo);

/* Movimentos Oficiais do Elo Maluco */
bool puzzle_rotacionar_superior_direita(EloMaluco* elo);  /* rsd */
bool puzzle_rotacionar_superior_esquerda(EloMaluco* elo); /* rse */
bool puzzle_rotacionar_inferior_direita(EloMaluco* elo);  /* rid */
bool puzzle_rotacionar_inferior_esquerda(EloMaluco* elo); /* rie */
bool puzzle_mover_face_cima(EloMaluco* elo);             /* mfc */
bool puzzle_mover_face_baixo(EloMaluco* elo);            /* mfb */

/* Execução de movimentos genéricos por string */
bool puzzle_executar_movimento(EloMaluco* elo, const char* acao);

/* Embaralhamento e Solução Demonstrativa */
void puzzle_embaralhar(EloMaluco* elo, int numMovimentos);
void puzzle_iniciar_solucao_demo(EloMaluco* elo);
void puzzle_proximo_passo_demo(EloMaluco* elo);

/* Atualização Física e Interpolação de Animações */
void puzzle_atualizar_animacao(EloMaluco* elo, float deltaTempo);

#endif /* PUZZLE_H */
