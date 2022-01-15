int pauseMenuN = 1, mainMenuN = 2, loadMenuN=-2, blinkState=0, optMenuN=0, high, loadSignal=0, loadDelayCounter=0, exitMenuN=1;
char nameStr[20], **hsdata, onStr[5]="ON", offStr[5]="OFF", victoryStr[80]="";
Color colLogo = {.R = 255, .G = 0, .B = 0}, colYellow = {.R=234, .G=180, .B=65};
MenuElement newGam, load, opt, hs, help;
MenuElement resume, save, quitMain, quit; 
MenuElement fullScreen, gameMusic, menuMusic, bgTexture;
MenuElement* mainMenu[6] = {&newGam, &load, &opt, &hs, &help, &quit};
MenuElement* pauseMenu[4] = {&resume, &save, &quitMain, &quit};
MenuElement saves[9];
MenuElement* optMenu[5] = {&fullScreen, &gameMusic, &menuMusic, &bgTexture, &saves[8]};
MenuBall *menuBalls;
MenuElement exitMenu[2];
void (*noFunc)(void);

void newGame(); void loadGame(int slot); void resumeGame(); void exitGame(); void saveGame(int slot); void setPauseMenu(); void setLoadMenu();
void setMenuElement (MenuElement *element, int n, Color *color, const char *x, void (*f)(void)); void showMenuElement (MenuElement *element); 
void setLoadMenuText(); void setSaveMenu(); void setScoreMenu(); void setNameInputMenu(); void setOptMenu(); void h10070rgb (double h, Color *rgb);
void setExitMenu(); void setQuitMenu();

void loadMenuBg() {
    int i; char path[50];
    for (i=1; i<=122; i++) {
        sprintf (path, "dxball/assets/menu/menuBG/%d", i);
        iLoadBMP (path, 100+i);
    };
    iLoadBMP ("dxball/assets/bg/1.bmp", 90);
    iLoadBMP ("dxball/assets/bg/2.bmp", 91);
    iLoadBMP ("dxball/assets/bg/3.bmp", 92);
    iLoadBMP ("dxball/assets/bg/4.bmp", 93);
    iLoadBMP ("dxball/assets/bg/5.bmp", 94);
    iLoadBMP ("dxball/assets/bg/6.bmp", 95);
};

void initMenuBalls() {
    int i, k;
    for (i=0; i<16; i++) {
        menuBalls[i].theta = rand()%360 / 180.0 * PI; menuBalls[i].speed = (135 + rand()%95)/10.0;
        menuBalls[i].dx = menuBalls[i].speed * cos(menuBalls[i].theta); menuBalls[i].dy = menuBalls[i].speed * sin(menuBalls[i].theta);
        printf("!");
        menuBalls[i].size = 16; menuBalls[i].pos.x = rand()%800; menuBalls[i].pos.y = rand()%600; 
        menuBalls[i].type = (BallType)(rand()%2);
        for (k=0; k<10; k++) {
            menuBalls[i].shardX[k] = 0; menuBalls[i].shardY[k] = 0;  
            menuBalls[i].shardVel[k] = 6 + rand()%6;
            menuBalls[i].shardAng[k] = (rand()%40)/40.0*360.0*PI/180;
        };
    };
};

void newGame() {
    int i; 
    life=3; score=0; paused=0; brickScore=0; level=1; levelSignal=0; levelDelayCounter=0; fireAnimState=0;
    snprintf (scoreStr, 34, "%06d", score);
    sprintf (cLvlStr, "LEVEL %02d", level);
    if (!settings.menuMusic && settings.gameMusic) {
        if (Mix_PausedMusic()) Mix_ResumeMusic();
    } else if (settings.menuMusic && !settings.gameMusic) {
        if (!Mix_PausedMusic()) Mix_PauseMusic();
    };
    balls = (Ball *)malloc(32*sizeof(Ball)); 
    levelReset();
    printf("...\n");
    paddle.shardAnimState = 0; paddle.pos = bottomCenter; paddle.pos.x -= paddle.size/2; paddle.sparkAnimStatus = 1;
    for (i=0; i<paddle.ballCount; i++) {
        balls[i].i = -1; balls[i].j = -1; balls[i].prevI = -1; balls[i].prevJ = -1;
        balls[i].pos.x = paddle.pos.x + paddle.size/2 - 5; 
        balls[i].pos.y = 10;
    };
    time (&startTime);
    iResumeTimer(0);
    Mix_PlayChannel(-1, Yay, 0);
    screen = GAME;
    setPauseMenu();
};

void loadGame (int slot) {
    if (!loadSignal) loadSignal = slot; 
    else {
        loadSignal = 0;
        levelDelayCounter=0; fireAnimState=0;
        if (!settings.menuMusic && settings.gameMusic) {
            if (Mix_PausedMusic()) Mix_ResumeMusic();
        } else if (settings.menuMusic && !settings.gameMusic) {
            if (!Mix_PausedMusic()) Mix_PauseMusic();
        };
        char savPath[50] = "dxball/userData/", s[12];
        strcat (savPath, itoa(slot, s, 10));
        strcat (savPath, ".SAV");
        int i; FILE *sav = fopen(savPath, "rb");
        brickMat = (Brick**) malloc(20*sizeof(Brick*));
        for (i=0; i<20; i++) brickMat[i] = (Brick*) malloc(20*sizeof(Brick));
        balls = (Ball *)malloc(32*sizeof(Ball)); 

        fread (&level, sizeof(int), 1, sav);
        sprintf (cLvlStr, "LEVEL %02d", level);
        fread (&life, sizeof(int), 1, sav);
        fread (&score, sizeof(int), 1, sav);
        fread (&scoreStr, 1, 35, sav);
        fread (&startTime, sizeof(time_t), 1, sav);
        fread (&gameTime, sizeof(time_t), 1, sav);
        fread (&timeStr, 1, 35, sav);
        fread (&brickScore, sizeof(int), 1, sav);
        fread (balls, sizeof(Ball), 32, sav);
        fread (&paddle, sizeof(Paddle), 1, sav);
        for (i=0; i<20; i++) fread (brickMat[i], sizeof(Brick), 20, sav);
        if (paddle.type == P_SHOOT || paddle.type == P_GRABSHOOT) {
            bullets = (Bullet *)malloc(100*sizeof(Bullet));
            fread (bullets, sizeof(Bullet), 100, sav);
        };
        fclose (sav);

        startTime += (time(NULL)-gameTime);
        Mix_PlayChannel(-1, Yay, 0);
        screen = GAME;
        iResumeTimer(0);
        setPauseMenu();
        paused = 0;
    };
};

void loadNoGame() { 
    Mix_PlayChannel(-1, Magnet, 0); 
};
void loadGame1() {loadGame(1);}; void loadGame2() {loadGame(2);}; void loadGame3() {loadGame(3);}; void loadGame4() {loadGame(4);};
void loadGame5() {loadGame(5);}; void loadGame6() {loadGame(6);}; void loadGame7() {loadGame(7);}; void loadGame8() {loadGame(8);};

void resumeGame() {
    if (!paused) {
        iPauseTimer(0);
        if (!settings.menuMusic && settings.gameMusic) Mix_PauseMusic();
    } else {
        if (!settings.menuMusic && settings.gameMusic) if (Mix_PausedMusic()) Mix_ResumeMusic();
        iResumeTimer(0);
    };
    paused = !paused;
};

void exitGame() {
    exit(0);
};

void saveGame (int slot) {
    int i; time_t currentTime;
    time (&currentTime);
    char savPath[50] = "dxball/userData/", s[12];
    strcat (savPath, itoa(slot, s, 10));
    strcat (savPath, ".SAV");
    FILE *sav = fopen(savPath, "wb");
    fwrite (&level, sizeof(int), 1, sav);
    fwrite (&life, sizeof(int), 1, sav);
    fwrite (&score, sizeof(int), 1, sav);
    fwrite (&scoreStr, 1, 35, sav);
    fwrite (&startTime, sizeof(time_t), 1, sav);
    fwrite (&gameTime, sizeof(time_t), 1, sav);
    fwrite (&timeStr, 1, 35, sav);
    fwrite (&brickScore, sizeof(int), 1, sav);
    fwrite (balls, sizeof(Ball), 32, sav);
    fwrite (&paddle, sizeof(Paddle), 1, sav);
    for (i=0; i<20; i++) fwrite (brickMat[i], sizeof(Brick), 20, sav);
    if (paddle.type == P_SHOOT || paddle.type == P_GRABSHOOT) fwrite (bullets, sizeof(Bullet), 100, sav);
    fwrite (&currentTime, sizeof(time_t), 1, sav);
    fclose (sav);
    setLoadMenuText();
};

void saveGame1() {saveGame(1);}; void saveGame2() {saveGame(2);}; void saveGame3() {saveGame(3);}; void saveGame4() {saveGame(4);};
void saveGame5() {saveGame(5);}; void saveGame6() {saveGame(6);}; void saveGame7() {saveGame(7);}; void saveGame8() {saveGame(8);};

void setMenuElement (MenuElement *element, int n, Color *color, const char *x, void (*f)(void)) {
    strcpy (element->str, x);
    (element->col) = *color;
    (element->region).A.x = 800/2 - strlen(element->str)*8;
    (element->region).B.x = 800/2 + strlen(element->str)*8;
    (element->region).A.y = 400-50*n;
    (element->region).B.y = 400-50*n + 22;
    element->n = n;
    element->func = f;
};

void showMenuElement (MenuElement *element, int menuN) {
    if (menuN == element->n) {
        iSetColor (220, 220, 220);
        if (blinkState) {
            iLine ((element->region).A.x-45, (element->region).A.y-8, (element->region).B.x+45, (element->region).A.y-8);
            iLine ((element->region).A.x-45, (element->region).B.y+4, (element->region).B.x+45, (element->region).B.y+4);
            iLine ((element->region).A.x-30, (element->region).A.y-9, (element->region).B.x+30, (element->region).A.y-9);
            iLine ((element->region).A.x-30, (element->region).B.y+5, (element->region).B.x+30, (element->region).B.y+5);
            iLine ((element->region).A.x-20, (element->region).A.y-10, (element->region).B.x+20, (element->region).A.y-10);
            iLine ((element->region).A.x-20, (element->region).B.y+6, (element->region).B.x+20, (element->region).B.y+6);
            iLine ((element->region).A.x-5, (element->region).A.y-11, (element->region).B.x+5, (element->region).A.y-11);
            iLine ((element->region).A.x-5, (element->region).B.y+7, (element->region).B.x+5, (element->region).B.y+7);
            iLine ((element->region).A.x, (element->region).A.y-12, (element->region).B.x, (element->region).A.y-12);
            iLine ((element->region).A.x, (element->region).B.y+8, (element->region).B.x, (element->region).B.y+8);
            iLine ((element->region).A.x+15, (element->region).A.y-13, (element->region).B.x-15, (element->region).A.y-13);
            iLine ((element->region).A.x+15, (element->region).B.y+9, (element->region).B.x-15, (element->region).B.y+9);
        };
    } else {
        iSetColor ((element->col).R, (element->col).G, (element->col).B);
        //iText ((element->region).A.x, (element->region).A.y, element->str, GLUT_BITMAP_TIMES_ROMAN_24);    
        //iRectangle ( (element->region).A.x, (element->region).A.y, ((element->region).B.x - (element->region).A.x), ((element->region).B.y - (element->region).A.y) );
    };
    iText ((element->region).A.x, (element->region).A.y, element->str, GLUT_BITMAP_TIMES_ROMAN_24);    
    iText ((element->region).A.x+1, (element->region).A.y, element->str, GLUT_BITMAP_TIMES_ROMAN_24);    
    iText ((element->region).A.x-1, (element->region).A.y, element->str, GLUT_BITMAP_TIMES_ROMAN_24);    
    iText ((element->region).A.x, (element->region).A.y+1, element->str, GLUT_BITMAP_TIMES_ROMAN_24);    
    iText ((element->region).A.x, (element->region).A.y-1, element->str, GLUT_BITMAP_TIMES_ROMAN_24);    
};

void backToGame() { screen = GAME; };
void setHelpMenu() { screen = HELPMENU; };

void setMainMenu() {
    iPauseTimer(0);
    if (!settings.menuMusic && settings.gameMusic) {
        if (!Mix_PausedMusic()) Mix_PauseMusic();
    } else if (settings.menuMusic) {
        if (Mix_PausedMusic()) Mix_ResumeMusic();
    };
    noFunc = &setMainMenu;
    setMenuElement (&newGam, 2, &colYellow, "NEW GAME ", &newGame);
    setMenuElement (&load, 3, &colYellow, "LOAD GAME ", &setLoadMenu);
    setMenuElement (&opt, 4, &colYellow, "OPTIONS", &setOptMenu);
    setMenuElement (&hs, 5, &colYellow, "HIGH SCORES", &setScoreMenu);
    setMenuElement (&help, 6, &colYellow, "HELP ", &setHelpMenu);
    setMenuElement (&quit, 7, &colYellow, "EXIT GAME", &setExitMenu);
    initMenuBalls();
    screen = MAINMENU;
};

void setPauseMenu() {
    noFunc = &backToGame;
    setMenuElement (&resume, 1, &colYellow, "RESUME", &resumeGame);
    setMenuElement (&save, 2, &colYellow, "SAVE GAME", &setSaveMenu);
    setMenuElement (&quitMain, 3, &colYellow, "MAIN MENU", &setQuitMenu);
    setMenuElement (&quit, 4, &colYellow, "EXIT GAME", &setExitMenu);
};


void setLoadMenuText () {
    int i, h; 
    for (i=0; i<8; i++) {
        int savLevel; time_t savTime;
        char savPath[50] = "dxball/userData/", s[12], timeStr[50], lvl[10], monStr[15];
        strcat (savPath, itoa(i+1, s, 10));
        strcat (savPath, ".SAV");
        FILE *sav = fopen(savPath, "rb");
        if (sav) {
            fread (&savLevel, sizeof(int), 1, sav);
            fseek (sav, 0x2BB86, 0);
            fread (&savTime, sizeof(time_t), 1, sav);
            fclose (sav);
            struct tm *timeStruct = localtime(&savTime);
            switch (timeStruct->tm_mon+1) {
                case 1: strcpy(monStr, "JANUARY"); break; case 2: strcpy(monStr, "FEBRUARY"); break; case 3: strcpy(monStr, "MARCH"); break;
                case 4: strcpy(monStr, "APRIL"); break; case 5: strcpy(monStr, "MAY"); break; case 6: strcpy(monStr, "JUNE"); break;
                case 7: strcpy(monStr, "JULY"); break; case 8: strcpy(monStr, "AUGUST"); break; case 9: strcpy(monStr, "SEPTEMBER"); break;
                case 10: strcpy(monStr, "OCTOBER"); break; case 11: strcpy(monStr, "NOVEMBER"); break; case 12: strcpy(monStr, "DECEMBER"); break;
            };
            if (timeStruct->tm_hour == 0) h = timeStruct->tm_hour + 12;
            else if (timeStruct->tm_hour > 12) h = timeStruct->tm_hour - 12;
            else h = timeStruct->tm_hour;
            sprintf (timeStr, "%02d-%s-%04d__%02d-%02d-%s", timeStruct->tm_mday, monStr, timeStruct->tm_year+1900, h, timeStruct->tm_min, (timeStruct->tm_hour > 11)?"PM":"AM" );
            strcpy (saves[i].str, "LEVEL-");
            sprintf (lvl, "%02d", savLevel);
            strcat (saves[i].str, lvl);
            strcat (saves[i].str, "__");
            strncat (saves[i].str, timeStr, 39);
            //strncat (saves[i].str, "    ", 4);
            saves[i].region.A.x = 800/2 - strlen(saves[i].str)*8;
            saves[i].region.B.x = 800/2 + strlen(saves[i].str)*8;
        } else {
            fclose (sav);
            if (saves[i].func==&loadGame1||saves[i].func==&loadGame2||saves[i].func==&loadGame3||saves[i].func==&loadGame4||saves[i].func==&loadGame5||saves[i].func==&loadGame6||saves[i].func==&loadGame7||saves[i].func==&loadGame8) {
                saves[i].func = &loadNoGame;
            };
        };
    };
};

void setLoadMenu() {
    screen = SAVEMENU;
    setMenuElement (&saves[0], -2, &colYellow, "EMPTY SAVE SLOT", &loadGame1);
    setMenuElement (&saves[1], -1, &colYellow, "EMPTY SAVE SLOT", &loadGame2);
    setMenuElement (&saves[2], 0, &colYellow, "EMPTY SAVE SLOT", &loadGame3);
    setMenuElement (&saves[3], 1, &colYellow, "EMPTY SAVE SLOT", &loadGame4);
    setMenuElement (&saves[4], 2, &colYellow, "EMPTY SAVE SLOT", &loadGame5);
    setMenuElement (&saves[5], 3, &colYellow, "EMPTY SAVE SLOT", &loadGame6);
    setMenuElement (&saves[6], 4, &colYellow, "EMPTY SAVE SLOT", &loadGame7);
    setMenuElement (&saves[7], 5, &colYellow, "EMPTY SAVE SLOT", &loadGame8);
    setMenuElement (&saves[8], 6, &colRock1, "BACK", &setMainMenu);
    setLoadMenuText();
};

void setSaveMenu() {
    screen = SAVEMENU;
    setMenuElement (&saves[0], -2, &colYellow, "EMPTY SAVE SLOT", &saveGame1);
    setMenuElement (&saves[1], -1, &colYellow, "EMPTY SAVE SLOT", &saveGame2);
    setMenuElement (&saves[2], 0, &colYellow, "EMPTY SAVE SLOT", &saveGame3);
    setMenuElement (&saves[3], 1, &colYellow, "EMPTY SAVE SLOT", &saveGame4);
    setMenuElement (&saves[4], 2, &colYellow, "EMPTY SAVE SLOT", &saveGame5);
    setMenuElement (&saves[5], 3, &colYellow, "EMPTY SAVE SLOT", &saveGame6);
    setMenuElement (&saves[6], 4, &colYellow, "EMPTY SAVE SLOT", &saveGame7);
    setMenuElement (&saves[7], 5, &colYellow, "EMPTY SAVE SLOT", &saveGame8);
    setMenuElement (&saves[8], 6, &colRock1, "BACK", &backToGame);
    setLoadMenuText();
};

void setScoreMenu() {
    int i; screen = SCOREMENU;
    hsdata = (char**)malloc(11*sizeof(char*));
    for (i=0; i<11; i++) hsdata[i] = (char*)malloc(150*sizeof(char));
    FILE *hsfile = fopen("dxball/userData/hsdata", "rb");
    for (i=0; i<10; i++) fread(hsdata[i], sizeof(char), 150, hsfile);
    setMenuElement (&saves[8], 7, &colRock1, "BACK", &setMainMenu);
    fclose (hsfile); 
};

// void blink() {
//     blinkState = !blinkState;
//     if (loadSignal) {
//         if (loadDelayCounter < 35) loadDelayCounter++;
//         else {
//             loadDelayCounter = 0;
//             loadGame (loadSignal);
//         };
//     };
// };

void bubbleSort (char **hsdata) {
    char *swap; int i, ok=0;
    while (ok != 10) {
        ok = 0;
        for (i=0; i < 10; i++) {
            char lvlStr[4] = {hsdata[i][22], hsdata[i][23]};
            char lvlStr1[4] = {hsdata[i+1][22], hsdata[i+1][23]};
            // printf("%s %s\n", lvlStr, lvlStr1);
            // printf("%d %d\n", atoi(lvlStr), atoi(lvlStr1));
            int lvl, lvl1;
            if (!strcmp(lvlStr1, "on")) lvl1 = 31;
            else lvl1 = atoi(lvlStr1);
            if (!strcmp(lvlStr, "on")) lvl = 31;
            else lvl = atoi(lvlStr);
            if ( lvl < lvl1 ){
                swap = hsdata[i];
                hsdata[i] = hsdata[i+1];
                hsdata[i+1] = swap;
            } else {
                ok += 1;
            };
        };
    };
};

void setNameInputMenu() {
    iPauseTimer(0);
    int i; high = 0;
    hsdata = (char**)malloc(11*sizeof(char*));
    for (i=0; i<11; i++) hsdata[i] = (char*)malloc(150*sizeof(char));
    FILE *hsfile = fopen("dxball/userData/hsdata", "rb");
    int delTime = (gameTime-startTime);
    //struct tm *gTime = localtime(&delTime);
    if (level==31) sprintf (hsdata[10], "/                    Won!       %06d        %s", score, timeStr);
    else sprintf (hsdata[10], "/                     %02d        %06d        %s", level, score, timeStr);
    for (i=0; i<10; i++) fread(hsdata[i], sizeof(char), 150, hsfile);
    bubbleSort (hsdata);
    for (i=0; i<10; i++) if (hsdata[i][0]=='/') high = i;
    fclose(hsfile);

    if (high) {
        screen = NAMEINPUT;
    } else {
        for (i=0; i<11; i++) free(hsdata[i]);
        free (hsdata);
        setScoreMenu();
    };
};


void f_fs() {
    settings.fullScreen = !settings.fullScreen; 
    if (settings.fullScreen) {
        glutDestroyWindow (glutGetWindow());
        glutGameModeString("800x600");
        glutEnterGameMode();
        glutCallbackInit();
    } else {
        glutLeaveGameMode();
        glutCreateWindow("DX-Ball : 1905084");
        glutCallbackInit();
    };
    setOptMenu(); 
};
void f_gm() {settings.gameMusic = !settings.gameMusic; setOptMenu(); };
void f_mm() {
    settings.menuMusic = !settings.menuMusic; 
    if (Mix_PausedMusic()) Mix_ResumeMusic();
    else Mix_PauseMusic();
    setOptMenu();
};
void f_bg() {settings.bgTexture = !settings.bgTexture; setOptMenu(); };


void setOptMenu() {
    char fsStr[50] = "FULLSCREEN WINDOW--", gmStr[50] = "GAME BACKGROUND MUSIC--", mmStr[50] = "MENU BACKGROUND MUSIC--", bgStr[50] = "BACKGROUND TEXTURE--";
    strcat(fsStr, settings.fullScreen?onStr:offStr);
    strcat(gmStr, settings.gameMusic?onStr:offStr);
    strcat(mmStr, settings.menuMusic?onStr:offStr);
    strcat(bgStr, settings.bgTexture?onStr:offStr);
    setMenuElement (&fullScreen, 0, &colYellow, fsStr, &f_fs);
    setMenuElement (&gameMusic, 1, &colYellow, gmStr, &f_gm);
    setMenuElement (&menuMusic, 2, &colYellow, mmStr, &f_mm);
    setMenuElement (&bgTexture, 3, &colYellow, bgStr, &f_bg);
    setMenuElement (&saves[8], 4, &colRock1, "BACK ", &setMainMenu);
    FILE *setfile = fopen ("dxball/userData/settings.cfg", "w");
    fprintf (setfile, "%d\n", settings.fullScreen);
    fprintf (setfile, "%d\n", settings.gameMusic);
    fprintf (setfile, "%d\n", settings.menuMusic);
    fprintf (setfile, "%d\n\n\n", settings.bgTexture);
    fprintf (setfile, "# ^ each line is a boolean value for the following settings:\n# line 1: fullscreen\n# line 2: play in-game music\n# line 3: play menu music\n# line 4: show background texture\n#");
    fclose(setfile);
    screen = OPTMENU;
};

void setExitMenu() {
    setMenuElement (&exitMenu[0], 1, &colYellow, "YES", &exitGame);
    setMenuElement (&exitMenu[1], 2, &colYellow, "NO", noFunc);
    screen = EXITCONF;
};

void setQuitMenu() {
    setMenuElement (&exitMenu[0], 1, &colYellow, "YES", &setMainMenu);
    setMenuElement (&exitMenu[1], 2, &colYellow, "NO", noFunc);
    screen = EXITCONF;
};

void h5075rgb (double H, Color *rgb) {
    double R, G, B;
    double S = 0.5, V = 0.75;
    double C = V * S;
    H /= 60.0;
    double X = C * (  1.0 - fabs( fmod(H,2.0) - 1.0 )  );
    double m = 1.0*(V-C);
    if (0<=H && H<1) {
        R=C; G=X; B=0;
    } else if (1<=H && H<2) {
        R=X; G=C; B=0;
    } else if (2<=H && H<3) {
        R=0; G=C; B=X;
    } else if (3<=H && H<4) {
        R=0; G=X; B=C;
    } else if (4<=H && H<5) {
        R=X; G=0; B=C;
    } else if (5<=H && H<6) {
        R=C; G=0; B=X;
    };
    rgb->R = (R+m)*255;
    rgb->G = (G+m)*255;
    rgb->B = (B+m)*255;
};