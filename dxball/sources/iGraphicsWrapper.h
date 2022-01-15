void mouseMoveHandlerFF2 (int mx, int my) {
    iMouseX = mx;
    iMouseY = iScreenHeight - my;
    iMouseMove(iMouseX, iMouseY);
    glFlush();
};

unsigned char **BMPstorage;

void iLoadBMP (char *file, int I) {
    FILE *bmp = fopen(file, "rb");
    fseek(bmp, 0, SEEK_END);
    int size = (int)ftell(bmp);
    fseek(bmp, 0, SEEK_SET);
    BMPstorage[I] = (unsigned char*)malloc((size+1) * sizeof(unsigned char));
    printf("\nB%d %d\n", I, size);
    fread (BMPstorage[I], size+1, 1, bmp); 
    fclose(bmp);
};

void iShowBMP3 (double x, double y, int I, Color *matte) {
    I-=2;
    int w = *((int*)(&BMPstorage[I][18])), h = *((int*)(&BMPstorage[I][22])), i, j, m , n, k;
    if ((3*w) % 4) k = floor(3*w/4)*4 + 4;
    else k = 3*w;
    for (m=0, i=0; i<h; m+=k, i++) {
        for (n=0, j=0; j<w; n+=3, j++) {
            if (!(BMPstorage[I][54+m+n]==matte->B &&  BMPstorage[I][55+m+n]==matte->G && BMPstorage[I][56+m+n]==matte->R)) {
                iSetColor(BMPstorage[I][56+m+n], BMPstorage[I][55+m+n], BMPstorage[I][54+m+n]);
                glBegin(GL_POINTS);
                glVertex2f(x+j, y+i);
                glEnd();
                //iFilledRectangle(x+j, y+i, 1, 1);
            };
        };
    };
};

void iShowBMP4 (double x, double y, int I) {
    I-=2;
    int w = *((int*)(&BMPstorage[I][18])), h = *((int*)(&BMPstorage[I][22])), i, j, m , n, k;
    if ((3*w) % 4) k = floor(3*w/4)*4 + 4;
    else k = 3*w;
    for (m=0, i=0; i<h; m+=k, i++) {
        for (n=0, j=0; j<w; n+=3, j++) {
            iSetColor(BMPstorage[I][56+m+n], BMPstorage[I][55+m+n], BMPstorage[I][54+m+n]);
            glBegin(GL_POINTS);
            glVertex2f(x+j, y+i);
            glEnd();
        };
    };
};

void iShowBMP4Hflip (double x, double y, int I) {
    I-=2;
    int w = *((int*)(&BMPstorage[I][18])), h = *((int*)(&BMPstorage[I][22])), i, j, m , n, k;
    if ((3*w) % 4) k = floor(3*w/4)*4 + 4;
    else k = 3*w;
    for (m=0, i=0; i<h; m+=k, i++) {
        for (n=0, j=w-1; j>=0; n+=3, j--) {
            iSetColor(BMPstorage[I][56+m+n], BMPstorage[I][55+m+n], BMPstorage[I][54+m+n]);
            glBegin(GL_POINTS);
            glVertex2f(x+j, y+i);
            glEnd();
        };
    };
};

void iShowBMP3Tex (double x, double y, int I, Color *matte, Color *col) {
    I-=2;
    int w = *((int*)(&BMPstorage[I][18])), h = *((int*)(&BMPstorage[I][22])), i, j, m , n, k;
    if ((3*w) % 4) k = floor(3*w/4)*4 + 4;
    else k = 3*w;
    for (m=0, i=0; i<h; m+=k, i++) {
        for (n=0, j=0; j<w; n+=3, j++) {
            if (!(BMPstorage[I][54+m+n]==matte->B &&  BMPstorage[I][55+m+n]==matte->G && BMPstorage[I][56+m+n]==matte->R)) {
                iSetColor (col->R*BMPstorage[I][56+m+n]/255.0, col->G*BMPstorage[I][55+m+n]/255.0, col->B*BMPstorage[I][54+m+n]/255.0);
                glBegin(GL_POINTS);
                glVertex2f(x+j, y+i);
                glEnd();
            };
        };
    };
};

void iShowBMPbg (int I) {
    int w = *((int*)(&BMPstorage[I][18])), h = *((int*)(&BMPstorage[I][22])), i, j, m , n, k;
    if ((3*w) % 4) k = floor(3*w/4)*4 + 4;
    else k = 3*w;
    for (m=0, i=0; i<h; m+=k, i++) {
        for (n=0, j=0; j<w; n+=3, j++) {
            iSetColor(BMPstorage[I][56+m+n], BMPstorage[I][55+m+n], BMPstorage[I][54+m+n]);
            iFilledRectangle(2*j, 2*i, 2, 2);
        };
    };
};

void iShowBMPbg2 (int I) {
    int w = *((int*)(&BMPstorage[I][18])), h = *((int*)(&BMPstorage[I][22])), i, j, m , n, k;
    if ((3*w) % 4) k = floor(3*w/4)*4 + 4;
    else k = 3*w;
    for (m=0, i=0; i<h; m+=k, i++) {
        for (n=0, j=0; j<w; n+=3, j++) {
            iSetColor(BMPstorage[I][56+m+n], BMPstorage[I][55+m+n], BMPstorage[I][54+m+n]);
            iFilledRectangle(4*j, 4*i, 4, 4);
        };
    };
};

void glutCallbackInit() {
    glutSetCursor(GLUT_CURSOR_NONE);
    glClearColor( 0.0 , 0.0 , 0.0 , 0.0 ) ;
    glMatrixMode( GL_PROJECTION) ;
    glLoadIdentity() ;
    glOrtho(0.0 , iScreenWidth , 0.0 , iScreenHeight , -1.0 , 1.0) ;

    iClear();

    glutDisplayFunc(displayFF) ;
    glutKeyboardFunc(keyboardHandler1FF);
    glutSpecialFunc(keyboardHandler2FF);
    glutMouseFunc(mouseHandlerFF);
    glutPassiveMotionFunc(mouseMoveHandlerFF);
    glutIdleFunc(animFF) ;

    glAlphaFunc(GL_GREATER,0.0f);
    glEnable(GL_ALPHA_TEST);

};

void iInitialize2 (int width, int height, char *title) {
    iScreenHeight = height;
    iScreenWidth = width;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA) ;
    glutInitWindowSize(width , height ) ;
    glutInitWindowPosition( 100 , 100 ) ;
    if (!settings.fullScreen) glutCreateWindow(title);
    else {
        glutGameModeString("800x600");
        glutEnterGameMode();
    };
    glutCallbackInit();
    glutMainLoop();
};