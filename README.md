# DX-Ball Reincarnation
This is a remake of the classic DX-Ball games made with C using the iGraphics library and SDL2.

![Preview of main menu](https://raw.githubusercontent.com/WJGalib/1-1_term_project_igraphics_dxball/main/preview-1.png)


![Preview of game](https://raw.githubusercontent.com/WJGalib/1-1_term_project_igraphics_dxball/main/preview-2.png)


To compile from source, run the following command in the project folder:

``` g++ -IOpenGL\include -Idxball\sources\include\SDL2 -w dxball.cpp -o dxball.exe -LOpenGL\lib -Ldxball\sources\lib -lmingw32 -lGlaux -lGLU32 -lglui32 -lglut32 -lOPENGL32 -lgdi32 -lSDL2main -lSDL2 -lSDL2_mixer -O3 -mwindows -static-libgcc -static-libstdc++ ```