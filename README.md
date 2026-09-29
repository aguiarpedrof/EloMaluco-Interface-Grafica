# 🧩 Elo Maluco 3D – Interface Gráfica Interativa com OpenGL / FreeGLUT

**UNIVERSIDADE FEDERAL DE ITAJUBÁ – UNIFEI**  
**Instituto de Ciências Tecnológicas – Engenharia de Computação**  
**ECOI24 – Computação Gráfica e Processamento Digital de Imagens**  
**Professor:** Dr. André Ribeiro de Brito  

---

## 📌 1. Visão Geral do Projeto

Este projeto consiste na implementação completa de uma aplicação gráfica 3D interativa em **linguagem C pura**, utilizando a biblioteca **OpenGL (versão Fixed-Function Pipeline) e FreeGLUT**. 

O tema da aplicação é a representação tridimensional do quebra-cabeça mecânico **Elo Maluco** (também conhecido internacionalmente como *Babylon Tower* ou *Rubik's Tower*). O objetivo do quebra-cabeça é organizar as peças coloridas de modo que cada coluna contenha uma única cor (Verde, Vermelho, Amarelo e Branco) e suas peças fiquem ordenadas por altura (Superior, Meio, Meio, Inferior), utilizando um único espaço vazio para permitir a movimentação.

A aplicação integra de forma harmoniosa todos os conceitos fundamentais de Computação Gráfica exigidos na disciplina:
- **Transformações Geométricas**: Translação, Rotação e Escala, aplicadas com hierarquia matricial (`glPushMatrix` / `glPopMatrix`).
- **Animações Fluidas**: Interpolação suave de movimentos (deslizamento vertical de peças e rotação angular de anéis).
- **Câmera Virtual e Visualização**: Câmera orbital 3D com controle esférico através de `gluLookAt`.
- **Projeção e Teste de Profundidade**: Projeção perspectiva dinâmica com `gluPerspective` e `glEnable(GL_DEPTH_TEST)`.
- **Mapeamento de Texturas**: Texturas 2D (mesa de madeira e moldura metálica) carregadas de arquivos `.bmp` sem dependências externas.
- **Iluminação e Materiais**: Modelo de iluminação Phong (componentes ambiente, difusa e especular) com propriedades de materiais reflexivos.
- **Interação Completa**: Controle por mouse (rotação orbital e zoom) e teclado (movimentações do jogo, embaralhamento, resolução e modos de câmera).

---

## 🎮 2. Controles e Interação

### 🔄 Movimentos do Elo Maluco:
| Tecla | Movimento | Descrição |
|:---:|:---:|---|
| **D** ou **→** | `rsd` | Rotacionar linha Superior para a Direita |
| **A** ou **←** | `rse` | Rotacionar linha Superior para a Esquerda |
| **L** | `rid` | Rotacionar linha Inferior para a Direita |
| **J** | `rie` | Rotacionar linha Inferior para a Esquerda |
| **W** ou **↑** | `mfc` | Mover Face para Cima (peça desliza para cima no espaço vazio) |
| **S** ou **↓** | `mfb` | Mover Face para Baixo (peça desliza para baixo no espaço vazio) |

### 🛠️ Funções do Jogo:
| Tecla | Função |
|:---:|---|
| **E** | **Embaralhar**: Executa sequência aleatória de movimentos válidos com animação |
| **R** | **Reiniciar**: Restaura o puzzle para o estado inicial resolvido |
| **Espaço** | **Resolver / Demo**: Executa automaticamente uma sequência de resolução animada |
| **V** | **Alternar Vista**: Alterna entre visualização Torre 3D e Vista Desdobrada (Planificada) |
| **H** | **HUD**: Oculta/Exibe o painel de instruções na tela |
| **ESC** | Sair da aplicação |

### 🎥 Câmera e Zoom:
| Controle | Ação |
|:---:|---|
| **Botão Esquerdo do Mouse + Arrastar** | Rotação orbital da câmera ao redor do centro do puzzle |
| **Scroll do Mouse** ou **[+] / [-]** | Zoom in / Zoom out da câmera (transformação de Escala) |
| **Botão Direito do Mouse** | Menu de contexto / Ações rápidas |

---

## 📐 3. Requisitos da Disciplina Atendidos

| Requisito do Edital | Como é Atendido na Aplicação |
|---|---|
| **Linguagem C e FreeGLUT** | Código desenvolvido 100% em C (padrão C99) com `GL/freeglut.h`. |
| **Objeto Gráfico Composto** | Puzzle modelado hierarquicamente com cilindro central ranhurado, anéis giratórios, 15 peças 3D com curvatura e moldura superior/inferior em uma mesa de suporte. |
| **Movimento / Animação** | Animação em tempo real (~60 FPS via `glutTimerFunc`), translação contínua das peças ao se moverem e rotação suave dos anéis superior/inferior. |
| **Translação** | `glTranslatef()` aplicado no reposicionamento dinâmico e interpolado das peças ao longo do eixo Y e da mesa de apoio. |
| **Rotação** | `glRotatef()` aplicado na rotação orbital da câmera, na rotação dos anéis mecânicos do puzzle e na orientação dos setores cilíndricos (0°, 90°, 180°, 270°). |
| **Escala** | `glScalef()` aplicado no zoom da câmera virtual, na diferenciação de altura/espessura das peças (superior, meio, inferior) e no efeito de pulsação luminosa/escala ao vencer o jogo. |
| **Textura** | Leitor de arquivos BMP em C puro; mapeamento de textura 2D com `glTexImage2D`, `glTexCoord2f` e filtros lineares na mesa de madeira e anéis de metal. |
| **Câmera Virtual** | `gluLookAt()` parametrizada com coordenadas esféricas (azimute, elevação e distância radial). |
| **Projeção Perspectiva** | `gluPerspective()` recalculada dinamicamente no callback de redimensionamento (`reshape`). |
| **Teste de Profundidade** | `glEnable(GL_DEPTH_TEST)` ativado com buffer de profundidade de 24/32 bits para eliminação de superfícies ocultas. |

---

## 💻 4. Como Compilar e Executar

### Pré-requisitos:
- Compilador GCC (MinGW-W64 no Windows ou GCC no Linux)
- Biblioteca FreeGLUT / OpenGL instalada

### No Windows:
Usando o script facilitador:
```cmd
build.bat
elo_maluco.exe
```
Ou manualmente via terminal:
```cmd
gcc -Wall -O2 src/elo_maluco.c -o elo_maluco.exe -lglut -lopengl32 -lglu32 -lm
elo_maluco.exe
```

### No Linux (Ubuntu / Debian / Mint):
```bash
sudo apt-get install freeglut3-dev build-essential
make
./elo_maluco
```
