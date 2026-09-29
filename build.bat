@echo off
echo =======================================================
echo Compilando Elo Maluco 3D - Computacao Grafica (ECOI24)
echo =======================================================

gcc -Wall -O2 src/elo_maluco.c src/puzzle.c src/camera.c src/render.c src/texture.c -o elo_maluco.exe -lglut -lopengl32 -lglu32 -lm

if %ERRORLEVEL% EQU 0 (
    echo [SUCESSO] Compilacao concluida com sucesso!
    echo Para executar: elo_maluco.exe
) else (
    echo [ERRO] Falha na compilacao. Verifique se o compilador GCC e FreeGLUT estao instalados.
)
