/**
 * @file texture.c
 * @brief Implementação do carregador de texturas BMP e mapeamento de texturas
 * Disciplina: ECOI24 - Computação Gráfica (UNIFEI)
 */

#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static GLuint g_texturas[TOTAL_TEXTURAS] = { 0 };

#pragma pack(push, 1)
typedef struct {
    unsigned short bfType;      /* 'BM' = 0x4D42 */
    unsigned int   bfSize;      /* Tamanho do arquivo */
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int   bfOffBits;   /* Offset dos dados de pixels */
} BmpFileHeader;

typedef struct {
    unsigned int   biSize;          /* 40 bytes */
    int            biWidth;
    int            biHeight;
    unsigned short biPlanes;        /* 1 */
    unsigned short biBitCount;      /* 24 bits */
    unsigned int   biCompression;   /* 0 = Sem compressão */
    unsigned int   biSizeImage;     /* Tamanho dos dados */
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
} BmpInfoHeader;
#pragma pack(pop)

GLuint texture_obter_id(TipoTextura tipo) {
    if (tipo < TOTAL_TEXTURAS) {
        return g_texturas[tipo];
    }
    return 0;
}

/* Carrega arquivo de textura BMP de 24 bits */
GLuint texture_carregar_bmp(const char* caminhoArquivo) {
    FILE* f = fopen(caminhoArquivo, "rb");
    if (!f) {
        return 0;
    }

    BmpFileHeader fileHeader;
    BmpInfoHeader infoHeader;

    if (fread(&fileHeader, sizeof(BmpFileHeader), 1, f) != 1 ||
        fread(&infoHeader, sizeof(BmpInfoHeader), 1, f) != 1) {
        fclose(f);
        return 0;
    }

    if (fileHeader.bfType != 0x4D42 || infoHeader.biBitCount != 24 || infoHeader.biCompression != 0) {
        fclose(f);
        return 0;
    }

    int largura = abs(infoHeader.biWidth);
    int altura = abs(infoHeader.biHeight);
    int rowPadded = (largura * 3 + 3) & (~3);

    unsigned char* linhaBuffer = (unsigned char*)malloc(rowPadded);
    unsigned char* imagemRGB = (unsigned char*)malloc(largura * altura * 3);

    if (!linhaBuffer || !imagemRGB) {
        free(linhaBuffer);
        free(imagemRGB);
        fclose(f);
        return 0;
    }

    fseek(f, fileHeader.bfOffBits, SEEK_SET);

    /* Leitura das linhas (de baixo para cima no BMP padrão) */
    for (int y = 0; y < altura; y++) {
        if (fread(linhaBuffer, rowPadded, 1, f) != 1) break;
        for (int x = 0; x < largura; x++) {
            /* BMP armazena em formato BGR -> convertemos para RGB */
            unsigned char b = linhaBuffer[x * 3 + 0];
            unsigned char g = linhaBuffer[x * 3 + 1];
            unsigned char r = linhaBuffer[x * 3 + 2];

            int idx = (y * largura + x) * 3;
            imagemRGB[idx + 0] = r;
            imagemRGB[idx + 1] = g;
            imagemRGB[idx + 2] = b;
        }
    }

    free(linhaBuffer);
    fclose(f);

    /* Criação da textura no OpenGL */
    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    /* Filtros linear para renderização de alta qualidade */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, imagemRGB);
    free(imagemRGB);

    return texId;
}

/* Gera e salva em disco uma imagem BMP procedural caso não exista */
void texture_gerar_e_salvar_bmp_padrao(const char* caminhoArquivo, TipoTextura tipo) {
    const int W = 256;
    const int H = 256;
    int rowPadded = (W * 3 + 3) & (~3);

    BmpFileHeader fh;
    BmpInfoHeader ih;
    memset(&fh, 0, sizeof(fh));
    memset(&ih, 0, sizeof(ih));

    fh.bfType = 0x4D42;
    fh.bfOffBits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);
    fh.bfSize = fh.bfOffBits + rowPadded * H;

    ih.biSize = sizeof(BmpInfoHeader);
    ih.biWidth = W;
    ih.biHeight = H;
    ih.biPlanes = 1;
    ih.biBitCount = 24;
    ih.biCompression = 0;
    ih.biSizeImage = rowPadded * H;

    FILE* f = fopen(caminhoArquivo, "wb");
    if (!f) return;

    fwrite(&fh, sizeof(BmpFileHeader), 1, f);
    fwrite(&ih, sizeof(BmpInfoHeader), 1, f);

    unsigned char* row = (unsigned char*)calloc(rowPadded, 1);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            unsigned char r = 0, g = 0, b = 0;

            if (tipo == TEXTURA_MESA_MADEIRA) {
                /* Simulação procedural de veios de madeira nobre */
                float dx = (float)(x - W / 2) * 0.05f;
                float dy = (float)(y - H / 2) * 0.05f;
                float dist = sqrtf(dx * dx + dy * dy);
                float ondulacao = sinf(dist * 6.0f + sinf((float)x * 0.15f) * 1.5f);
                float fator = 0.5f + 0.5f * ondulacao;

                /* Tons de madeira carvalho/nogueira */
                r = (unsigned char)(115.0f + fator * 45.0f);
                g = (unsigned char)(70.0f  + fator * 35.0f);
                b = (unsigned char)(35.0f  + fator * 25.0f);
            } else {
                /* Textura metálica escovada */
                float ruido = (float)(rand() % 40) - 20.0f;
                float base = 150.0f + sinf((float)y * 0.8f) * 15.0f + ruido;
                if (base < 0.0f) {
                    base = 0.0f;
                }
                if (base > 255.0f) {
                    base = 255.0f;
                }
                r = (unsigned char)base;
                g = (unsigned char)(base * 0.98f);
                b = (unsigned char)(base * 1.05f);
            }

            /* Salva BGR */
            row[x * 3 + 0] = b;
            row[x * 3 + 1] = g;
            row[x * 3 + 2] = r;
        }
        fwrite(row, rowPadded, 1, f);
    }

    free(row);
    fclose(f);
}

void texture_inicializar(void) {
    /* Habilita mapeamento de textura 2D e modulação com iluminação */
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    const char* arqMadeira = "textura_madeira.bmp";
    const char* arqMetal   = "textura_metal.bmp";

    /* Garante existência dos arquivos de textura */
    FILE* fTeste = fopen(arqMadeira, "rb");
    if (!fTeste) {
        texture_gerar_e_salvar_bmp_padrao(arqMadeira, TEXTURA_MESA_MADEIRA);
    } else {
        fclose(fTeste);
    }

    fTeste = fopen(arqMetal, "rb");
    if (!fTeste) {
        texture_gerar_e_salvar_bmp_padrao(arqMetal, TEXTURA_METAL_ANEL);
    } else {
        fclose(fTeste);
    }

    /* Carrega as texturas no contexto OpenGL */
    g_texturas[TEXTURA_MESA_MADEIRA] = texture_carregar_bmp(arqMadeira);
    g_texturas[TEXTURA_METAL_ANEL]   = texture_carregar_bmp(arqMetal);

    glDisable(GL_TEXTURE_2D);
}
