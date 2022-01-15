void iMouseMove(int mx, int my) {
    int nx;
    switch (screen) {
        case GAME: {
                int z;
                double prevX = paddle.pos.x;
                if (!paused) {
                    if (my!=300) glutWarpPointer(mx, 300);
                    if (mx<paddle.size/2) glutWarpPointer(paddle.size/2, 300);
                    if (mx>iScreenWidth-paddle.size/2) glutWarpPointer(iScreenWidth-paddle.size/2, 300);
                    if (mx>=paddle.size/2 && mx<=iScreenWidth-paddle.size/2 && !levelSignal) {
                        paddle.pos.x = mx - paddle.size/2;
                        if (paddle.resetStatus) for (z=0; z<paddle.ballCount; z++) balls[z].pos.x += (paddle.pos.x - prevX);
                    };
                } else {
                    if (my < (pauseMenu[3]->region).A.y+2) glutWarpPointer (400, 600-(pauseMenu[3]->region).A.y-2);
                    if (my > (pauseMenu[0]->region).A.y+48) glutWarpPointer (400, 600-(pauseMenu[0]->region).A.y-48);
                    if (mx != 400) glutWarpPointer (400, 600-my);
                
                    nx = (600-my)/50 - 3;
                    printf("%d\n", nx);
                    if (nx >= 1 && nx <= 4 && pauseMenuN != nx) {
                        Mix_PlayChannel(-1, Boop, 0);
                        pauseMenuN = nx;
                    };
                };
            }; break;

         case MAINMENU: 
            if (my < (mainMenu[5]->region).A.y+2) glutWarpPointer (400, 600-(mainMenu[5]->region).A.y-2);
            if (my > (mainMenu[0]->region).A.y+48) glutWarpPointer (400, 600-(mainMenu[0]->region).A.y-48);
            if (mx != 400) glutWarpPointer (400, 600-my);
           
            nx = (600-my)/50 - 3;
            printf("%d\n", nx);
            if (nx >= 2 && nx <= 7 && mainMenuN != nx) {
                Mix_PlayChannel(-1, Boop, 0);
                mainMenuN = nx;
            }; break;
                
        case SAVEMENU:
            if (my < saves[8].region.A.y+2) glutWarpPointer (400, 600-saves[8].region.A.y-2);
            if (my > saves[0].region.A.y+48) glutWarpPointer (400, 600-saves[0].region.A.y-48);
            if (mx != 400) glutWarpPointer (400, 600-my);
            
            if (!loadSignal) {
                nx = (600-my)/50 - 3;
                printf("%d\n", nx);
                if (nx >= -2 && nx <= 6 && loadMenuN != nx) {
                    Mix_PlayChannel(-1, Boop, 0);
                    loadMenuN = nx;
                };
            }; break;

        case OPTMENU: 
            if (my < (optMenu[4]->region).A.y+2) glutWarpPointer (400, 600-(optMenu[4]->region).A.y-2);
            if (my > (optMenu[0]->region).A.y+48) glutWarpPointer (400, 600-(optMenu[0]->region).A.y-48);
            if (mx != 400) glutWarpPointer (400, 600-my);
           
            nx = (600-my)/50 - 3;
            printf("%d\n", nx);
            if (nx >= 0 && nx <= 4 && optMenuN != nx) {
                Mix_PlayChannel(-1, Boop, 0);
                optMenuN = nx;
            }; break;

        case EXITCONF:
            if (my < exitMenu[1].region.A.y+2) glutWarpPointer (400, 600-exitMenu[1].region.A.y-2);
            if (my > exitMenu[0].region.A.y+48) glutWarpPointer (400, 600-exitMenu[0].region.A.y-48);
            if (mx != 400) glutWarpPointer (400, 600-my);
            
            nx = (600-my)/50 - 3;
            printf("%d\n", nx);
            if (nx >= 1 && nx <= 2 && exitMenuN != nx) {
                Mix_PlayChannel(-1, Boop, 0);
                exitMenuN = nx;
            }; break;
    };
};

void iMouse(int button, int state, int mx, int my) {
    int i, j;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && !levelSignal) {
        switch (screen) {
            case GAME:
                if (!paused) {
                    if (paddle.resetStatus) paddle.resetStatus = 0;
                    if (paddle.type == P_SHOOT || paddle.type == P_GRABSHOOT) {
                        Mix_PlayChannel(-1, Shoot, 0);
                        if (paddle.lastBullet == -1) {
                            bullets[0] = { .pos={.x = paddle.pos.x+8, .y = 20}, .active=1 };
                            bullets[1] = { .pos={.x = paddle.pos.x+paddle.size-10, .y = 20}, .active=1 };
                            paddle.lastBullet = 2;
                        } else {
                            bullets[paddle.lastBullet] = { .pos={.x = paddle.pos.x+8, .y = 20}, .active=1 };
                            bullets[paddle.lastBullet+1] = { .pos={.x = paddle.pos.x+paddle.size-10, .y = 20}, .active=1 };                
                            paddle.lastBullet += 2;
                        };
                        if (paddle.lastBullet >=97) paddle.lastBullet = -1;
                    };
                } else {
                    Mix_PlayChannel(-1, Boop, 0);
                    pauseMenu[pauseMenuN-1]->func();
                }; break;
            
            case MAINMENU: 
                Mix_PlayChannel(-1, Boop, 0);
                mainMenu[mainMenuN-2]->func(); break;
                
            case SAVEMENU:
                if (!loadSignal) {
                    Mix_PlayChannel(-1, Boop, 0);
                    saves[loadMenuN+2].func(); 
                }; break;

            case OPTMENU: 
                Mix_PlayChannel(-1, Boop, 0);
                optMenu[optMenuN]->func(); break;

            case SCOREMENU:
                Mix_PlayChannel(-1, Boop, 0);
                strcpy (victoryStr, "");
                setMainMenu(); break;

            case EXITCONF:
                Mix_PlayChannel(-1, Boop, 0);
                exitMenu[exitMenuN-1].func(); break;
        };
	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && !levelSignal) {
        switch (screen) {
            case GAME: 
                Mix_PlayChannel(-1, Boop, 0);
                resumeGame(); break;
            case MAINMENU: break;
            case EXITCONF:
                Mix_PlayChannel(-1, Boop, 0);
                noFunc(); break;
            default: 
                if (!loadSignal) {
                    Mix_PlayChannel(-1, Boop, 0);
                    setMainMenu();
                };
        };
    };
};

void iKeyboard(unsigned char key) {
    switch (screen) {
        case MAINMENU: 
            if (key=='\r') {
                Mix_PlayChannel(-1, Boop, 0);
                mainMenu[mainMenuN-2]->func();
            }; break;
        case SAVEMENU:
            if (!loadSignal) {
                if (key=='\r') {
                    Mix_PlayChannel(-1, Boop, 0);
                    saves[loadMenuN+2].func();
                }; 
                if (key=='\b' || key==27) {
                    Mix_PlayChannel(-1, Boop, 0);
                    saves[8].func();
                }; 
            }; break;
        case GAME:
            if (key=='p' || key==27 || key=='\b') {
                if (!paused) {
                    iPauseTimer(0);
                    if (!settings.menuMusic && settings.gameMusic) Mix_PauseMusic();
                } else {
                    if (!settings.menuMusic && settings.gameMusic) if (Mix_PausedMusic()) Mix_ResumeMusic();
                    iResumeTimer(0);
                };
                paused = !paused;
            };
            if (!paused) {
                if (key==' ' && paddle.resetStatus && !levelSignal) {
                    paddle.resetStatus = !paddle.resetStatus;
                };
            } else {
                if (key=='\r') {
                    Mix_PlayChannel(-1, Boop, 0);
                    pauseMenu[pauseMenuN-1]->func();
                };
            }; break;
        case SCOREMENU:
            if (key=='\r' || key=='\b' || key==27) {
                Mix_PlayChannel(-1, Boop, 0);
                strcpy (victoryStr, "");
                setMainMenu();
            }; break;
        case NAMEINPUT:
            if (key=='\r') {
                int i;
                strncpy (hsdata[high], nameStr, strlen(nameStr));
                FILE *hsfile = fopen("dxball/userData/hsdata", "wb");
                for (i=0; i<10; i++) fwrite(hsdata[i], sizeof(char), 150, hsfile);
                fclose(hsfile);
                for (i=0; i<11; i++) free(hsdata[i]);
                free (hsdata);
                setScoreMenu();
            } else if (key == '\b') {
                Mix_PlayChannel(-1, Boop, 0);
                if (strlen(nameStr) > 0) nameStr[strlen(nameStr)-1] = '\0';
            } else if (key=='-' || key>='A' && key<='Z' || key>='a' && key<='z' || key>='0' && key<='9' || key=='.' || key=='_' || key==' ') {
                Mix_PlayChannel(-1, Magnet, 0);
                if (strlen(nameStr) < 14) {
                    nameStr[strlen(nameStr)+1] = '\0';
                    nameStr[strlen(nameStr)] =  key;
                };
            }; break;
        case OPTMENU: 
            if (key=='\r' || key==' ') {
                Mix_PlayChannel(-1, Boop, 0);
                optMenu[optMenuN]->func();
            } else if (key=='\b' || key==27) {
                Mix_PlayChannel(-1, Boop, 0);
                setMainMenu();
            }; break;
        case HELPMENU:
            if (key=='\r' || key=='\b' || key==27) {
                Mix_PlayChannel(-1, Boop, 0);
                setMainMenu();
            }; break;
        case EXITCONF: 
            if (key=='\r') {
                Mix_PlayChannel(-1, Boop, 0);
                exitMenu[exitMenuN-1].func();
            } else if (key=='\b' || key==27) {
                Mix_PlayChannel(-1, Boop, 0);
                noFunc();
            }; break;
        default:
            if (key=='\r' || key=='\b' || key==27) {
                Mix_PlayChannel(-1, Boop, 0);
                setMainMenu();
            };
    };
	
};


void iSpecialKeyboard (unsigned char key) {
    int z;
	if (key==GLUT_KEY_END) exit(0);
    switch (screen) {
        case MAINMENU: 
            if (key==GLUT_KEY_UP) {
                if (mainMenuN > 2) mainMenuN--;
                else mainMenuN = 7;
                Mix_PlayChannel(-1, Boop, 0);
            };
            if (key==GLUT_KEY_DOWN) {
                if (mainMenuN < 7) mainMenuN++;
                else mainMenuN = 2;
                Mix_PlayChannel(-1, Boop, 0);
            }; 
            break;
        
        case SAVEMENU:
            if (!loadSignal) {
                if (key==GLUT_KEY_UP) {
                    if (loadMenuN > -2) loadMenuN--;
                    else loadMenuN = 6;
                    Mix_PlayChannel(-1, Boop, 0);
                };
                if (key==GLUT_KEY_DOWN) {
                    if (loadMenuN < 6) loadMenuN++;
                    else loadMenuN = -2;
                    Mix_PlayChannel(-1, Boop, 0);
                }; 
            }; break;

        case OPTMENU:
            if (key==GLUT_KEY_UP) {
                if (optMenuN > 0) optMenuN--;
                else optMenuN = 4;
                Mix_PlayChannel(-1, Boop, 0);
            };
            if (key==GLUT_KEY_DOWN) {
                if (optMenuN < 4) optMenuN++;
                else optMenuN = 0;
                Mix_PlayChannel(-1, Boop, 0);
            }; break;

        case EXITCONF:
            if (key==GLUT_KEY_UP || key==GLUT_KEY_DOWN) {
                if (exitMenuN == 1) exitMenuN = 2;
                else exitMenuN = 1;
                Mix_PlayChannel(-1, Boop, 0);
            }; break;

        case GAME: 
             if (!paused) {
                if (key==GLUT_KEY_LEFT && paddle.pos.x>0 && !paused && !levelSignal) {
                    double DX;
                    if (paddle.pos.x < 30) DX = paddle.pos.x;
                    else DX = 30;
                    paddle.pos.x -= DX;
                    if (paddle.resetStatus) for (z=0; z<paddle.ballCount; z++) balls[z].pos.x -= DX;
                };
                if (key==GLUT_KEY_RIGHT && paddle.pos.x<iScreenWidth-paddle.size && !paused && !levelSignal) {
                    double DX;
                    if (paddle.pos.x > iScreenWidth-paddle.size-30) DX = iScreenWidth - paddle.size - paddle.pos.x;
                    else DX = 30;
                    paddle.pos.x += DX;
                    if (paddle.resetStatus) for (z=0; z<paddle.ballCount; z++) balls[z].pos.x += DX;
                };
                if (key==GLUT_KEY_PAGE_UP && !paused && !levelSignal) levelSignal = 1;
                if (key==GLUT_KEY_HOME && !paused && !levelSignal) paddleShoot();
                if (key==GLUT_KEY_INSERT && !paused && !levelSignal) fireBall();
                if (key==GLUT_KEY_PAGE_DOWN && !paused && !levelSignal) throughBricks();
                if (key==GLUT_KEY_F4 && !paused && !levelSignal) splitBall();
                if (key==GLUT_KEY_F5 && !paused && !levelSignal) eightBall();
                if (key==GLUT_KEY_F6 && !paused && !levelSignal) zapBricks();
            } else {
                if (key==GLUT_KEY_UP) {

                    if (pauseMenuN > 1) pauseMenuN--;
                    else pauseMenuN = 4;
                    Mix_PlayChannel(-1, Boop, 0);
                };
                if (key==GLUT_KEY_DOWN) {
                    if (pauseMenuN < 4) pauseMenuN++;
                    else pauseMenuN = 1;
                    Mix_PlayChannel(-1, Boop, 0);
                };
            };
            break;
    };
   
};