#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "dxball/sources/definitions.h"
#include "iGraphics.h"
#include "dxball/sources/iGraphicsWrapper.h"
#include "dxball/sources/include/SDL2/SDL.h"
#include "dxball/sources/include/SDL2/SDL_mixer.h"

#define PI 3.14159265358979

enum Screen {MAINMENU, SAVEMENU, OPTMENU, SCOREMENU, HELPMENU, GAME, NAMEINPUT, EXITCONF} screen = MAINMENU;
int life, score, paused, brickScore, level, levelSignal, levelDelayCounter, fireAnimState, bgFireAnimState=1;
char scoreStr[35] = "000000", timeStr[35], cLvlStr[5];
time_t startTime, gameTime, levelTime;
Color colOrange = {.R=254, .G=200, .B=45}, colBlack = {.R=0, .G=0, .B=0}, colMatte = {.R=170, .G=170, .B=170},
      colRock1 = {.R=64, .G=64, .B=64}, colRock2 = {.R=128, .G=128, .B=128}, colRock3 = {.R=192, .G=192, .B=192};
Point O = {.x=0, .y=0}, bottomCenter = {.x = 800/2 - 5, .y=10};
Ball *balls; Brick **brickMat; Paddle paddle; Bullet *bullets;
Mix_Chunk *Bang, *Boop, *Boing, *Byeball, *Pew, *Wall, *FastBoing, *Magnet, *Shoot, *Metal, *Invisible, *Tada, *Yay, *Fall, *Grow;
Mix_Music *bgm1;

void gameLoop();

void loadBMP() {
    int i, num;
    FILE *bmplist = fopen("dxball/data/loadBMP", "r");
    char **list = (char**)malloc(100*sizeof(char*));
    fscanf(bmplist, "%d", &num);
    for (i=0; i<num-1; i++) {
        list[i] = (char*)malloc(200*sizeof(char));
        fscanf(bmplist, "%s", list[i]);
        //printf("_%s_", list[i]);
        iLoadBMP(list[i], i);
        free(list[i]);
    };
    fclose(bmplist);
    //printf("%d", num);
    free(list);
};

void loadBricks (int level) {
    int i, j, k, m, n;
    char lvldir[35] = "dxball/data/levels/", lvl[30];
    strcat(lvldir, itoa(level, lvl, 10));
    strcat(lvldir, ".LVL");
    FILE *bmp = fopen(lvldir, "rb");
    unsigned char *data = (unsigned char*)malloc(1255*sizeof(unsigned char));
    fread (data, 1255, 1, bmp); 
    fclose(bmp);
    for (m=1200-60, i=0; m>=0, i<20; m-=60, i++) {
        for (n=0, j=0; j<20; n+=3, j++) {
            if (data[54+m+n]+data[55+m+n]+data[56+m+n] == 0) {
                brickMat[i][j].type = NONE;
                brickScore++;
            } else if (data[54+m+n]==0 && data[55+m+n]==0 && data[56+m+n]==255) brickMat[i][j].type = EXPLODING;
            else if (data[54+m+n]+data[55+m+n]+data[56+m+n] == 255*3) {
                brickMat[i][j].type = INVISIBLE;
                brickMat[i][j].textureImg = 70;
            } else if (data[54+m+n]==64 && data[55+m+n]==64 && data[56+m+n]==64) brickMat[i][j].type = ROCK_1;
            else if (data[54+m+n]==128 && data[55+m+n]==128 && data[56+m+n]==128) brickMat[i][j].type = ROCK_2;
            else if (data[54+m+n]==192 && data[55+m+n]==192 && data[56+m+n]==192) brickMat[i][j].type = ROCK_3;
            else if (data[54+m+n]==0 && data[55+m+n]==255 && data[56+m+n]==255) brickMat[i][j].type = METAL;
            else {
                brickMat[i][j].type = NORMAL;
                brickMat[i][j].textureImg = 71;
            };

            if (brickMat[i][j].type != EXPLODING) {
                brickMat[i][j].col.R = (double)data[56+m+n]; 
                brickMat[i][j].col.G = (double)data[55+m+n];
                brickMat[i][j].col.B = (double)data[54+m+n];
            } else brickMat[i][j].col = {.R=255, .G=200, .B=0};

            brickMat[i][j].pos.x = j*40; brickMat[i][j].pos.y = 520-i*20;
            brickMat[i][j].shardAnimState = 0;
            if (brickMat[i][j].type != NONE) {
                for (k=0; k<10; k++) {
                    brickMat[i][j].shardX[k] = 0; brickMat[i][j].shardY[k] = 0;  
                    brickMat[i][j].shardVel[k] = 6 + rand()%6;
                    brickMat[i][j].shardAng[k] = ((rand()%40)/40.0*360.0)*PI/180;
                };
            };
        };
        printf("\n");
    };
    free(data);
};

void lifeReset() {
    int k;
    paddle.resetStatus = 1; paddle.ballCount = 1; paddle.size = 100;
    paddle.brickFallState = 0; paddle.type = P_NORMAL; //paddle.shardAnimState = 0;
    balls[0].dx=0; balls[0].dy = balls[0].speed = 6, balls[0].theta = PI/2;    
    balls[0].size = 10; balls[0].pos.x = paddle.pos.x + paddle.size/2 - 5; balls[0].pos.y = 10; balls[0].type = T_NORMAL;
    for (k=0; k<50; k++) {
            paddle.shardX[k] = 0; paddle.shardY[k] = 0;  
            paddle.shardVel[k] = 6 + rand()%6;
            paddle.shardAng[k] = (rand()%80)/80.0*180.0*PI/180;
    };
};

void levelReset() {
    printf("a1");
    int i, j; 
    printf("a2");
    levelDelayCounter = 0;
    printf("a3");
    if (paddle.type == P_SHOOT || paddle.type == P_GRABSHOOT) {
        paddle.lastBullet = -1;
        free(bullets);
    };
    printf("a4");
    lifeReset();
    printf("a5");
    paddle.bottomRow = 19;
    printf("a6");
    brickMat = (Brick**) malloc(20*sizeof(Brick*));
    printf("a7");
    for (i=0; i<20; i++) brickMat[i] = (Brick*) malloc(20*sizeof(Brick));
    printf("a8");
    loadBricks(level);
    printf("a9");
};

#include "dxball/sources/menu.h"

void brickBreak (int i, int j) {
    printf("break: %d %d\n", i, j);
    if (brickMat[i][j].type != NONE) {
        if (brickMat[i][j].type == EXPLODING) Mix_PlayChannel(-1, Bang, 0);
        else Mix_PlayChannel(-1, Boop, 0);
        brickMat[i][j].type = NONE;
        brickMat[i][j].shardAnimState = 1;
        //brickScore++; 
        if (balls[0].type == T_FIRE) score += 16;
        else if (balls[0].type == T_THRU) score += 14;
        else {
            if ((gameTime-startTime)/60.0 > 2.5) score += 12;
            else if ((gameTime-startTime)/60.0 > 2) score += 11;
            else if ((gameTime-startTime)/60.0 > 1.5) score += 10;
            else if ((gameTime-startTime)/60.0 > 1) score += 9;
            else score += 8;
        };
        snprintf (scoreStr, 34, "%06d", score);
    };
    printf("R: %d\n", brickScore);
};

void brickMakeVisible (int i, int j) {
    printf("break: %d %d\n", i, j);
    Mix_PlayChannel(-1, Invisible, 0);
    brickMat[i][j].type = NORMAL;
    brickMat[i][j].shardAnimState = 1;
};

void brickExplode (int i, int j) {
    brickBreak(i,j);
    if (i-1>=0) {
        if (j-1>=0) {
            if (brickMat[i-1][j-1].type==EXPLODING) brickExplode(i-1, j-1);
            else brickBreak(i-1, j-1);
        };
        if (j+1<20) {
            if (brickMat[i-1][j+1].type==EXPLODING) brickExplode(i-1, j+1);
            else brickBreak(i-1, j+1);
        };
        if (brickMat[i-1][j].type==EXPLODING) brickExplode(i-1, j);
        else brickBreak(i-1, j);
    };
    if (i+1<20) {
        if (j-1>=0) {
            if (brickMat[i+1][j-1].type==EXPLODING) brickExplode(i+1, j-1);
            else brickBreak(i+1, j-1);
        };
        if (j+1<20) {
            if (brickMat[i+1][j+1].type==EXPLODING) brickExplode(i+1, j+1);
            else brickBreak(i+1, j+1);
        };
        if (brickMat[i+1][j].type==EXPLODING) brickExplode(i+1, j);
        else brickBreak(i+1, j);
    };
    if (j-1>=0) {
        if (brickMat[i][j-1].type==EXPLODING) brickExplode(i, j-1);
        else brickBreak(i, j-1);
    };
    if (j-1<20) {
        if (brickMat[i][j+1].type==EXPLODING) brickExplode(i, j+1);
        else brickBreak(i, j+1);
    };
};

void removeBall (int n) {
    int i;
    if (n == paddle.ballCount-1) paddle.ballCount--;
    else {
        for (i=n; i<paddle.ballCount-1; i++) balls[i]=balls[i+1];
        paddle.ballCount--;
    };
};

int ballOnEdge (int side, int i, int j, int z) {
    double bumpX = balls[z].pos.x + balls[z].size/2, bumpY = balls[z].pos.y + balls[z].size/2;
    switch (side) {
        case 1: if (fabs(brickMat[i][j].pos.x+40 - bumpX) <= balls[z].size/2 && bumpY >= brickMat[i][j].pos.y && bumpY <= brickMat[i][j].pos.y + 20) 
                return 1; else return 0; break; 
        case 3: if (fabs(brickMat[i][j].pos.x - bumpX) <= balls[z].size/2 && bumpY >= brickMat[i][j].pos.y && bumpY <= brickMat[i][j].pos.y + 20) 
                return 1; else return 0; break; 
        case 2: if (fabs(brickMat[i][j].pos.y+20 - bumpY) <= balls[z].size/2 && bumpX >= brickMat[i][j].pos.x && bumpX <= brickMat[i][j].pos.x + 40) 
                return 1; else return 0; break; 
        case 4: if (fabs(brickMat[i][j].pos.y - bumpY) <= balls[z].size/2 && bumpX >= brickMat[i][j].pos.x && bumpX <= brickMat[i][j].pos.x + 40) 
                return 1; else return 0; break; 
    };
};

int ballInBrick (int z, int i, int j) {
    if (i<0 || i>=20 || j<0 || j>=20) return 0;
    else {
        if ( (ballOnEdge(1,i,j,z) || ballOnEdge(2,i,j,z) || ballOnEdge(3,i,j,z) || ballOnEdge(4,i,j,z)) && brickMat[i][j].type!=NONE ) return 1;
        else return 0;
    };
};

void findIJ (int z) {
    if ( ballInBrick(z, balls[z].i-1, balls[z].j) ) balls[z].i--;
    else if ( ballInBrick(z, balls[z].i, balls[z].j+1) ) balls[z].j++;
    else if ( ballInBrick(z, balls[z].i+1, balls[z].j) ) balls[z].i++;
    else if ( ballInBrick(z, balls[z].i, balls[z].j-1) ) balls[z].j--;
    else if ( ballInBrick(z, balls[z].i-1, balls[z].j+1) ) {
        balls[z].j++; balls[z].i--;
    } else if ( ballInBrick(z, balls[z].i+1, balls[z].j+1) ) {
        balls[z].j++; balls[z].i++;
    } else if ( ballInBrick(z, balls[z].i-1, balls[z].j-1) ) {
        balls[z].j--; balls[z].i--;
    } else if ( ballInBrick(z, balls[z].i+1, balls[z].j-1) ) {
        balls[z].j--; balls[z].i++;
    };
};

#include "dxball/sources/animFuncs.h"
#include "dxball/sources/powerups.h"

void gameLoop() {

    if (levelSignal) {
        printf("b1\n");
        if (!Mix_PausedMusic()) {
            Mix_PauseMusic();            
        };
        printf("b2\n");
        levelDelayCounter++;
        printf("b3");
        if (levelDelayCounter >= 200) {
            Mix_PlayChannel(-1, Yay, 0);
            printf("b4\n");
            levelSignal = 0;
            printf("b5\n");
            int k;
            for (k=0; k<20; k++) free(brickMat[k]);
            free(brickMat);
            printf("b6\n");
            level++;
            printf("b7\n");
            sprintf (cLvlStr, "LEVEL %02d", level);
            printf("b8\n");
            if (level==31) {
                strcpy (victoryStr, "You have completed all 30 levels of the entire game!");
                Mix_PlayChannel(-1, Tada, 0);
                setNameInputMenu();
            } else levelReset();
            if (settings.gameMusic) Mix_ResumeMusic();
        };
    };

    if (!paddle.resetStatus) {
        int z;
        for (z=0; z<paddle.ballCount; z++) {
            if (balls[z].pos.x >= iScreenWidth-balls[z].size) {
                Mix_PlayChannel(-1, Wall, 0);
                balls[z].dx = -fabs(balls[z].dx);
                balls[z].theta = atan2(balls[z].dy, balls[z].dx);
            } else if (balls[z].pos.x <= 0) {
                Mix_PlayChannel(-1, Wall, 0);
                balls[z].dx = fabs(balls[z].dx);
                balls[z].theta = atan2(balls[z].dy, balls[z].dx);
            };

            if (balls[z].pos.y >= iScreenHeight-balls[z].size) {
                Mix_PlayChannel(-1, Wall, 0);
                balls[z].dy = -fabs(balls[z].dy);
                balls[z].theta = atan2(balls[z].dy, balls[z].dx);
            };
            if (balls[z].pos.y <=10 && balls[z].pos.x >= paddle.pos.x-balls[z].size && balls[z].pos.x <= paddle.pos.x+paddle.size) {
                if ((paddle.type == P_GRAB || paddle.type == P_GRABSHOOT) &&  balls[z].pos.x >= paddle.pos.x+5 && balls[z].pos.x <= paddle.pos.x+paddle.size-balls[z].size-5) {
                    Mix_PlayChannel(-1, Magnet, 0);
                    paddle.resetStatus = 1; //balls[z].dx=0; balls[z].dy=0; balls[z].speed = 6;
                } else {
                    if (balls[z].speed<10) Mix_PlayChannel(-1, Boing, 0);
                    else Mix_PlayChannel(-1, FastBoing, 0);
                };
                if (paddle.brickFallState) {
                    int ix, jx, kx=0;
                    for (ix=19; ix>=0; ix--) {
                        for (jx=19; jx>=0; jx--) {
                            if (brickMat[ix][jx].type != NONE) {
                                paddle.bottomRow = ix; 
                                kx = 1; break;
                            };
                        };
                        if (kx) break;
                    };
                    if (brickMat[paddle.bottomRow][19].pos.y > 100) {
                        for (ix=0; ix<=paddle.bottomRow; ix++) {
                            for (jx=0; jx<20; jx++) {
                                brickMat[ix][jx].pos.y -= 20;
                            };
                        };
                        Mix_PlayChannel(-1, Fall, 0);
                        printf("BR: %d", paddle.bottomRow);
                    };
                };
                balls[z].theta = atan2(balls[z].pos.y+balls[z].size/2, (balls[z].pos.x+balls[z].size/2 - paddle.pos.x - paddle.size/2));
                balls[z].dy = balls[z].speed * sin(balls[z].theta); balls[z].dx = balls[z].speed * cos(balls[z].theta);  
            };
            if (balls[z].pos.y <= 0) removeBall(z);
            if (paddle.ballCount == 0) paddleKill();

            #define IJ_SAFE (balls[z].i>=0 && balls[z].i<20 && balls[z].j>=0 && balls[z].j<20 && brickMat[balls[z].i][balls[z].j].type != NONE)
            balls[z].prevI = balls[z].i; balls[z].prevJ = balls[z].j;
            balls[z].i = paddle.bottomRow + 1 - (balls[z].pos.y + balls[z].size/2 - brickMat[paddle.bottomRow][19].pos.y)/20.0;
            balls[z].j = (balls[z].pos.x + balls[z].size/2) / 40;

            findIJ (z);
          
            if IJ_SAFE {
                
                if (balls[z].type != T_THRU) {
                    if (abs(balls[z].i-balls[z].prevI)>=1) balls[z].dy = -balls[z].dy;
                    if (abs(balls[z].j-balls[z].prevJ)>=1) balls[z].dx = -balls[z].dx;
                    balls[z].theta = atan2(balls[z].dy, balls[z].dx);
                };
                if (balls[z].type == T_FIRE) brickMat[balls[z].i][balls[z].j].type = EXPLODING;
                if (balls[z].type == T_THRU) {
                    if (brickMat[balls[z].i][balls[z].j].type == EXPLODING) brickExplode(balls[z].i, balls[z].j);
                    else brickBreak(balls[z].i, balls[z].j);
                } else if (brickMat[balls[z].i][balls[z].j].type == NORMAL || brickMat[balls[z].i][balls[z].j].type == ROCK_1) {
                    brickBreak(balls[z].i, balls[z].j);
                } else if (brickMat[balls[z].i][balls[z].j].type == ROCK_2) {
                    brickMat[balls[z].i][balls[z].j].shardAnimState = 1;
                    brickMat[balls[z].i][balls[z].j].type = ROCK_1;
                    brickMat[balls[z].i][balls[z].j].col = colRock1;
                    Mix_PlayChannel(-1, Metal, 0);
                } else if (brickMat[balls[z].i][balls[z].j].type == ROCK_3) {
                    brickMat[balls[z].i][balls[z].j].shardAnimState = 1;
                    brickMat[balls[z].i][balls[z].j].type = ROCK_2;
                    brickMat[balls[z].i][balls[z].j].col = colRock2;
                    Mix_PlayChannel(-1, Metal, 0);
                } else if (brickMat[balls[z].i][balls[z].j].type == METAL) {
                    brickMat[balls[z].i][balls[z].j].shardAnimState = 1;
                    Mix_PlayChannel(-1, Metal, 0);
                    brickMat[balls[z].i][balls[z].j].col = colOrange;
                } else if (brickMat[balls[z].i][balls[z].j].type == EXPLODING) brickExplode(balls[z].i, balls[z].j);
                else if (brickMat[balls[z].i][balls[z].j].type == INVISIBLE) brickMakeVisible(balls[z].i, balls[z].j);
                int powerUpChance = rand()%7;
                if (powerUpChance == 4) spawnPowerUp(balls[z].i, balls[z].j);
            };
            //printf("%d %d\n", abs(balls[z].i-balls[z].prevI), abs(balls[z].j-balls[z].prevJ));
        
            if (!levelSignal) {
                balls[z].pos.x += balls[z].dx;
                balls[z].pos.y += balls[z].dy;
            };
            
            if (balls[z].type == T_FIRE || balls[z].type == T_THRU) {
                balls[z].trailAnimState = !balls[z].trailAnimState;
                if (balls[z].trailAnimState) animateTrails(z);
            };
        };
    
        for (int r=0; r<paddle.lastBullet; r++) {
            bullets[r].i = paddle.bottomRow - (bullets[r].pos.y - brickMat[paddle.bottomRow][19].pos.y)/20.0;
            //bullets[r].i = 20 - (bullets[r].pos.y+20.0 -120.0)/20.0;
            bullets[r].j = (bullets[r].pos.x+1.0)/40.0;
            if (bullets[r].i>=0 && bullets[r].i<20 && bullets[r].j>=0 && bullets[r].j<20 && brickMat[bullets[r].i][bullets[r].j].type != NONE) {
                if (brickMat[bullets[r].i][bullets[r].j].type == INVISIBLE) brickMakeVisible(bullets[r].i, bullets[r].j);
                else if (brickMat[bullets[r].i][bullets[r].j].type == EXPLODING) brickExplode(bullets[r].i, bullets[r].j);
                else if (brickMat[bullets[r].i][bullets[r].j].type == METAL) {
                    brickMat[bullets[r].i][bullets[r].j].shardAnimState = 1;
                    brickMat[bullets[r].i][bullets[r].j].col = colOrange;
                } else if (brickMat[bullets[r].i][bullets[r].j].type != METAL) {
                    brickBreak(bullets[r].i, bullets[r].j);
                    score -= 6;
                };
                bullets[r].active = 0;
            };
        };
    
    } else {
        paddle.sparkAnimStatus = !paddle.sparkAnimStatus;
        if (paddle.sparkAnimStatus) animateSpark();
    };

    for (int r=0; r<paddle.lastBullet; r++) if (bullets[r].active) bullets[r].pos.y += 7;    
    
    animateProjectiles();
    if (paddle.shardAnimState) animatePaddleShards();

    if (fireAnimState == 120) fireAnimState = 0;
    else fireAnimState++;

    if (brickScore==400) levelSignal = 1;

    time (&gameTime);
    snprintf(timeStr, 34, "%02d:%02d:%02d", (gameTime-startTime)/3600, (gameTime-startTime)/60, (gameTime-startTime)%60);

};


#include "dxball/sources/drawFuncs.h"
#include "dxball/sources/ioFuncs.h"

int main (int argc, char **argv) {
    int i, j;

    FILE *setfile = fopen ("dxball/userData/settings.cfg", "r");
    fscanf (setfile, "%d", &settings.fullScreen);
    fscanf (setfile, "%d", &settings.gameMusic);
    fscanf (setfile, "%d", &settings.menuMusic);
    fscanf (setfile, "%d", &settings.bgTexture);
    fclose(setfile);

    SDL_Init (SDL_INIT_AUDIO);
    Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, 2, 2048);
    bgm1 = Mix_LoadMUS ("dxball/assets/sounds/bgm1.mp3");
    Bang = Mix_LoadWAV ("dxball/assets/sounds/Bang.wav");
    Boing = Mix_LoadWAV ("dxball/assets/sounds/Boing.wav");
    Boop = Mix_LoadWAV ("dxball/assets/sounds/Boop.wav");
    Byeball = Mix_LoadWAV ("dxball/assets/sounds/Byeball.wav");
    Pew = Mix_LoadWAV ("dxball/assets/sounds/Pew.wav");
    Wall = Mix_LoadWAV ("dxball/assets/sounds/Wall.wav");
    FastBoing = Mix_LoadWAV ("dxball/assets/sounds/FastBoing.wav");
    Magnet = Mix_LoadWAV ("dxball/assets/sounds/Magnet.wav");
    Metal = Mix_LoadWAV ("dxball/assets/sounds/Metal.wav");
    Invisible = Mix_LoadWAV ("dxball/assets/sounds/Invisible.wav");
    Shoot = Mix_LoadWAV ("dxball/assets/sounds/Shoot.wav");
    Tada = Mix_LoadWAV ("dxball/assets/sounds/Tada.wav");
    Yay = Mix_LoadWAV ("dxball/assets/sounds/Yay.wav");
    Fall = Mix_LoadWAV ("dxball/assets/sounds/Fall.wav");
    Grow = Mix_LoadWAV ("dxball/assets/sounds/Grow.wav");

    Mix_PlayMusic (bgm1, -1);
    Mix_PauseMusic();

    BMPstorage = (unsigned char**)malloc(300*sizeof(unsigned char*));
    menuBalls = (MenuBall*)malloc(16*sizeof(MenuBall));
    loadBMP();
    iSetTimer(5, gameLoop); 
    //iSetTimer (200, blink); 
    iSetTimer(10, animateMenu);
    
    loadMenuBg();
    setMainMenu();

    iInitialize2(800, 600, "DX-Ball : 1905084");
    
    Mix_FreeMusic(bgm1);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
};