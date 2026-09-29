/**
 * @file texture.h
 * @brief Gerenciador e carregador de texturas 2D (arquivos BMP) em C puro
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef enum {
    TEXTURA_MESA_MADEIRA = 0,
    TEXTURA_METAL_ANEL = 1,
    TOTAL_TEXTURAS
} TipoTextura;

/* Inicializa o sistema de texturas e carrega/gera as texturas necessárias */
void texture_inicializar(void);

/* Retorna o ID OpenGL da textura especificada */
GLuint texture_obter_id(TipoTextura tipo);

/* Carrega um arquivo BMP de 24 bits da raiz ou caminho relativo */
GLuint texture_carregar_bmp(const char* caminhoArquivo);

/* Gera texturas procedurais de alta qualidade caso o arquivo não exista */
void texture_gerar_e_salvar_bmp_padrao(const char* caminhoArquivo, TipoTextura tipo);

#endif /* TEXTURE_H */
