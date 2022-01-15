void animateBrickShards(int i, int j) {
    int k;
    for(k=0; k<10; k++) {
        brickMat[i][j].shardX[k] += ( brickMat[i][j].shardVel[k] * cos(brickMat[i][j].shardAng[k]) );
        brickMat[i][j].shardY[k] += ( brickMat[i][j].shardVel[k] * sin(brickMat[i][j].shardAng[k]) );
    };
    if (brickMat[i][j].shardX[5]>=200 || brickMat[i][j].shardX[5]<=-200 || brickMat[i][j].shardY[5]>=200 || brickMat[i][j].shardY[5]<=-200) {
        brickMat[i][j].shardAnimState = 0;
        if (brickMat[i][j].type == METAL || brickMat[i][j].type == ROCK_2 || brickMat[i][j].type == ROCK_3)
        for (k=0; k<10; k++) {
            brickMat[i][j].shardX[k] = 0; brickMat[i][j].shardY[k] = 0;  
            brickMat[i][j].shardVel[k] = 6 + rand()%6;
            brickMat[i][j].shardAng[k] = (rand()%40)/40.0*360.0*PI/180;
        };
    };
};

void animatePaddleShards() {
    int k;
    for(k=0; k<50; k++) {
        paddle.shardX[k] += ( paddle.shardVel[k] * cos(paddle.shardAng[k]) );
        paddle.shardY[k] += ( paddle.shardVel[k] * sin(paddle.shardAng[k]) );
    };
    if (paddle.shardX[5]>=200 || paddle.shardX[5]<=-200 || paddle.shardY[5]>=200 || paddle.shardY[5]<=-200) {
        paddle.shardAnimState = 0;
    };
}

void animateSpark() {
    paddle.spark[0] = {.x = -2, .y = 24}; 
    paddle.spark[19] = {.x = paddle.size + 2, .y = 24};
    double unit = paddle.size/19; 
    int i, del = unit/2 + 1;
    for (i=1; i<19; i++) {
        paddle.spark[i].x = unit * i +  (rand()%2?-1:1) * rand()%del;
        paddle.spark[i].y = 18 + rand()%14;
    };
};

void animateTrails (int z) {
    balls[z].trails[0] = {.x = 0, .y = 0}; 
    double unit = 50/10, dr, dt; 
    int i, del = unit/2+1;
    for (i=1; i<10; i++) {
        dr = rand()%del; 
        dt = (rand()%27 + 1.3*i)/3;
        balls[z].trails[i].x = cos(balls[z].theta) * (unit * i + dr) + (rand()%2?-1:1) * dt * sin(balls[z].theta);
        balls[z].trails[i].y = sin(balls[z].theta) * (unit * i + dr) + (rand()%2?-1:1) * dt * cos(balls[z].theta);
    };
};

void animateProjectiles() {
    int p, q, rx = 0;
    for (p=0; p<20; p++) {
        for (q=0; q<20; q++) {
            if (brickMat[p][q].type == NONE) rx++;
            brickScore = rx;
            if (brickMat[p][q].shardAnimState) animateBrickShards(p, q);
            if (brickMat[p][q].power.active) {
                if (brickMat[p][q].power.y<=10) {
                    if (brickMat[p][q].power.x >= paddle.pos.x-30  &&  brickMat[p][q].power.x <= paddle.pos.x+paddle.size-2) {
                        Mix_PlayChannel(-1, Pew, 0);
                        score += 100;
                        brickMat[p][q].power.type();
                    };
                    if (brickMat[p][q].power.y < 30) brickMat[p][q].power.active = 0;
                };
                brickMat[p][q].power.vy -= 0.05;
                if (brickMat[p][q].power.x >= iScreenWidth-32) {
                    brickMat[p][q].power.vx = -fabs(brickMat[p][q].power.vx);
                } else if (brickMat[p][q].power.x <= 0) {
                    brickMat[p][q].power.vx = fabs(brickMat[p][q].power.vx);
                };

                brickMat[p][q].power.x += brickMat[p][q].power.vx;
                brickMat[p][q].power.y += brickMat[p][q].power.vy;
            };
        };
    };
};

void animateBgFire() {
    if (bgFireAnimState == 121) bgFireAnimState = 1;
    else bgFireAnimState++;
    h5075rgb (((bgFireAnimState-1)*3)%360 * 1.0, &colLogo);
};

void animateMenuBalls() {
    int i, k, z;
    for (z=0; z<16; z++) {
        if (menuBalls[z].pos.x <= 0) {
            menuBalls[z].dx = fabs(menuBalls[z].dx);
            menuBalls[z].shardAnimState = 1;
            menuBalls[z].theta = atan2 (menuBalls[z].dy, menuBalls[z].dx);
        } else if (menuBalls[z].pos.x >= iScreenWidth) {
            menuBalls[z].dx = -fabs(menuBalls[z].dx);
            menuBalls[z].shardAnimState = 1;
            menuBalls[z].theta = atan2 (menuBalls[z].dy, menuBalls[z].dx);
        } if (menuBalls[z].pos.y <= 0) {
            menuBalls[z].dy = fabs(menuBalls[z].dy);
            menuBalls[z].shardAnimState = 1;
            menuBalls[z].theta = atan2 (menuBalls[z].dy, menuBalls[z].dx);
        } else if (menuBalls[z].pos.y >= iScreenHeight) {
            menuBalls[z].dy = -fabs(menuBalls[z].dy);
            menuBalls[z].shardAnimState = 1;
            menuBalls[z].theta = atan2 (menuBalls[z].dy, menuBalls[z].dx);
        };

        if (menuBalls[z].shardAnimState) {
            for(k=0; k<10; k++) {
                menuBalls[z].shardX[k] += ( menuBalls[z].shardVel[k] * cos(menuBalls[z].shardAng[k]) );
                menuBalls[z].shardY[k] += ( menuBalls[z].shardVel[k] * sin(menuBalls[z].shardAng[k]) );
            };
            if (menuBalls[z].shardX[5]>=200 || menuBalls[z].shardX[5]<=-200 || menuBalls[z].shardY[5]>=200 || menuBalls[z].shardY[5]<=-200) {
                menuBalls[z].shardAnimState = 0;
                for (k=0; k<10; k++) {
                    menuBalls[z].shardX[k] = 0; menuBalls[z].shardY[k] = 0;  
                    menuBalls[z].shardVel[k] = 6 + rand()%6;
                    menuBalls[z].shardAng[k] = (rand()%40)/40.0*360.0*PI/180;
                };
            };
        };

        menuBalls[z].trails[0] = {.x = 0, .y = 0}; 
        double unit = 50/10, dr, dt; 
        int del = unit/2+1;
        for (i=1; i<10; i++) {
            dr = rand()%del; 
            dt = (rand()%27 + 1.3*i)/3;
            menuBalls[z].trails[i].x = cos(menuBalls[z].theta) * (unit * i + dr) + (rand()%2?-1:1) * dt * sin(menuBalls[z].theta);
            menuBalls[z].trails[i].y = sin(menuBalls[z].theta) * (unit * i + dr) + (rand()%2?-1:1) * dt * cos(menuBalls[z].theta);
        };

        menuBalls[z].pos.x += menuBalls[z].dx;
        menuBalls[z].pos.y += menuBalls[z].dy;

        //printf("%lg %lg\n", menuBalls[z].dx, menuBalls[z].dy);
    };
};

void animateMenu() {
    if (!(screen==GAME && !paused)) animateMenuBalls();
    animateBgFire();
    if (!(bgFireAnimState%10)) blinkState = !blinkState;
    if (loadSignal) {
        if (loadDelayCounter < 280) loadDelayCounter++;
        else {
            loadDelayCounter = 0;
            loadGame (loadSignal);
        };
    };
};