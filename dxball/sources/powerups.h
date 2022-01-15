void extraLife() {
    if (life<=5) life++;
}; //1

void fastBall() {
    int z;
    for (z=0; z<paddle.ballCount; z++) {
        balls[z].speed = 12;
        balls[z].dx = balls[z].speed * cos(balls[z].theta);
        balls[z].dy = balls[z].speed * sin(balls[z].theta);
    };
}; //2

void slowBall() {
    int z;
    for (z=0; z<paddle.ballCount; z++) {
        balls[z].speed = 3;
        balls[z].dx = balls[z].speed * cos(balls[z].theta);
        balls[z].dy = balls[z].speed * sin(balls[z].theta);
    };
}; //3

void paddleExpand() {
    double prev = paddle.size;
    if (paddle.size<99) paddle.size = 100;
    else if (paddle.size<101 && paddle.size>99) paddle.size = 190;
    else if (paddle.size<191 && paddle.size>189) paddle.size = 280;
    else if (paddle.size<281 && paddle.size>279) paddle.size = 340;
    paddle.pos.x -= ((paddle.size-prev)/2);
    if (paddle.pos.x<0) paddle.pos.x = 0;
    else if (paddle.pos.x + paddle.size/2 > iScreenWidth) paddle.pos.x = iScreenWidth - paddle.size/2;
}; //4

void paddleShrink() {
    double prev = paddle.size;
    if (paddle.size<=100 && paddle.size>46) paddle.size = 70;
    else if (paddle.size<191 && paddle.size>189) paddle.size = 100;
    else if (paddle.size<21 && paddle.size>279) paddle.size = 190;
    else if (paddle.size>339) paddle.size = 280;
    paddle.pos.x -= ((paddle.size-prev)/2);
    if (paddle.pos.x<0) paddle.pos.x = 0;
    else if (paddle.pos.x + paddle.size/2 > iScreenWidth) paddle.pos.x = iScreenWidth - paddle.size/2;
}; //5

void throughBricks() {
    int z;
    for (z=0; z<paddle.ballCount; z++) balls[z].type = T_THRU;
}; //6

void fireBall() {
    int z;
    for (z=0; z<paddle.ballCount; z++) {
        balls[z].type = T_FIRE;
        balls[z].size = 10;
    };
}; //7

void explodMultiply() {
    int i, j;
    for (i=0; i<20; i++) {
        for (j=0; j<20; j++) {
            if (brickMat[i][j].type == EXPLODING) {
                if (i-1>=0) {
                    if (j-1>=0 && brickMat[i-1][j-1].type!=NONE) brickMat[i-1][j-1].type = EXPLODING;
                    if (j+1<20 && brickMat[i-1][j+1].type!=NONE) brickMat[i-1][j+1].type = EXPLODING;
                    if (brickMat[i-1][j].type!=NONE) brickMat[i-1][j].type = EXPLODING;
                };
                if (i+1<20) {
                    if (j-1>=0 && brickMat[i+1][j-1].type!=NONE) brickMat[i+1][j-1].type = EXPLODING;
                    if (j+1<20 && brickMat[i+1][j+1].type!=NONE) brickMat[i+1][j+1].type = EXPLODING;
                    if (brickMat[i+1][j].type!=NONE) brickMat[i+1][j].type = EXPLODING;
                };
                if (j-1>=0 && brickMat[i][j-1].type!=NONE) brickMat[i][j-1].type = EXPLODING;
                if (j+1<20 && brickMat[i][j+1].type!=NONE) brickMat[i][j+1].type = EXPLODING;
            };
        };
    };
}; //8

void explodDetonate() {
    int i, j;
    for (i=0; i<20; i++) {
        for (j=0; j<20; j++) {
            if (brickMat[i][j].type == EXPLODING) brickExplode(i, j);
        };
    };
}; //9

void zapBricks() {
    int i, j;
    for (i=0; i<20; i++) {
        for (j=0; j<20; j++) {
            if (brickMat[i][j].type == METAL) brickMat[i][j].type = ROCK_1;
            else if (brickMat[i][j].type == ROCK_3 || brickMat[i][j].type == ROCK_2) {
                brickMat[i][j].type = ROCK_1;
                brickMat[i][j].col = colRock1;
            } else if (brickMat[i][j].type == INVISIBLE) brickMat[i][j].type = NORMAL;
        };
    };
}; //10

void levelSkip() {
    levelSignal = 1;
}; //11



void paddleSuperShrink() {
    double prev = paddle.size;
    paddle.size = 40;
    paddle.pos.x -= ((paddle.size-prev)/2);
    if (paddle.pos.x<0) paddle.pos.x = 0;
    else if (paddle.pos.x + paddle.size/2 > iScreenWidth) paddle.pos.x = iScreenWidth - paddle.size/2;
}; //12

void paddleKill() {
    paddle.shardAnimState = 1;
    if (life>0) {
        life--;
        lifeReset();
    } else setNameInputMenu();
    Mix_PlayChannel(-1, Byeball, 0);                
}; //13

void expandBall() {
    int z;
    Mix_PlayChannel(-1, Grow, 0);
    for (z=0; z<paddle.ballCount; z++) balls[z].size = 16;
}; //14

void shrinkBall() {
    int z;
    Mix_PlayChannel(-1, Grow, 0);
    for (z=0; z<paddle.ballCount; z++) balls[z].size = 6;
}; //15

void paddleGrab() {
    paddle.ballCount = 1;
    if (paddle.type == P_SHOOT) paddle.type = P_GRABSHOOT;
    else paddle.type = P_GRAB;
}; //16

void paddleShoot() {
    int i, j; 
    for (i=0; i<20; i++) {
        for (j=0; j<20; j++) {
            if (brickMat[i][j].type == ROCK_3 || brickMat[i][j].type == ROCK_2) brickMat[i][j].type = ROCK_1;
            else if (brickMat[i][j].type == INVISIBLE) brickMat[i][j].type = NORMAL;
        };
    };
    if (paddle.type == P_GRAB) paddle.type = P_GRABSHOOT;
    else paddle.type = P_SHOOT;
    paddle.lastBullet = -1;
    bullets = (Bullet *)malloc(100*sizeof(Bullet));
}; //17

void splitBall() {
    int i;
    paddle.resetStatus = 0;
    if (paddle.type == P_GRAB) paddle.type = P_NORMAL;
    else if (paddle.type == P_GRABSHOOT) paddle.type = P_SHOOT;
    if (paddle.ballCount <= 16) {
        paddle.ballCount *= 2;
        for (i=paddle.ballCount/2; i>0; i--) balls[2*i-1] = balls[i-1];
        for (i=paddle.ballCount-2; i>=0; i-=2) {
            balls[i] = balls[i+1];
            balls[i].theta += ((rand()%2?-1:1) * PI/3);
            balls[i].dx = balls[i].speed * cos(balls[i].theta);
            balls[i].dy = balls[i].speed * sin(balls[i].theta);
        };
    };
}; //18

void eightBall() {
    int i, z;
    paddle.resetStatus = 0;
    if (paddle.type == P_GRAB) paddle.type = P_NORMAL;
    else if (paddle.type == P_GRABSHOOT) paddle.type = P_SHOOT;
    if (paddle.ballCount <= 4) {
        paddle.ballCount *= 8;
        fastBall();
        for (i=paddle.ballCount/8; i>0; i--) balls[8*i-1] = balls[i-1];
        for (i=paddle.ballCount-8; i>=0; i-=8) {
            for (z=i; z<i+7; z++) {
                balls[z] = balls[i+7];
                if ( fabs(cos(fabs(fmod(balls[z].theta, 360)))-1)<=0.008 || fabs(sin(fabs(fmod(balls[z].theta, 360)))-1)<=0.008 ) 
                    balls[z].theta += (PI*5.0/180.0);
                balls[z].theta += ((z-i)*2*PI/paddle.ballCount);
                balls[z].dx = balls[z].speed * cos(balls[z].theta);
                balls[z].dy = balls[z].speed * sin(balls[z].theta);
            };
        };
    };
}; //19

void fallingBricks() {
    paddle.brickFallState = 1;
}; //20


void spawnPowerUp(int i, int j) {
    brickMat[i][j].power.x = brickMat[i][j].pos.x + 20; brickMat[i][j].power.y = brickMat[i][j].pos.y + 10; 
    brickMat[i][j].power.vx = 1.5 + (rand()%30)*0.1; brickMat[i][j].power.vy = 1.5 + abs(rand()%3);
    brickMat[i][j].power.image = 1 + abs(rand()%20);
    switch (brickMat[i][j].power.image) {
        case  1: brickMat[i][j].power.type = &extraLife; break;
        case  2: brickMat[i][j].power.type = &fastBall; break;
        case  3: brickMat[i][j].power.type = &slowBall; break;
        case  4: brickMat[i][j].power.type = &paddleExpand; break;
        case  5: brickMat[i][j].power.type = &paddleShrink; break;
        case  6: brickMat[i][j].power.type = &throughBricks; break;
        case  7: brickMat[i][j].power.type = &fireBall; break;
        case  8: brickMat[i][j].power.type = &explodMultiply; break;
        case  9: brickMat[i][j].power.type = &explodDetonate; break;
        case 10: brickMat[i][j].power.type = &zapBricks; break;
        case 11: brickMat[i][j].power.type = &levelSkip; break;
        case 12: brickMat[i][j].power.type = &paddleSuperShrink; break;
        case 13: brickMat[i][j].power.type = &paddleKill; break;
        case 14: brickMat[i][j].power.type = &expandBall; break;
        case 15: brickMat[i][j].power.type = &shrinkBall; break;
        case 16: brickMat[i][j].power.type = &paddleGrab; break;
        case 17: brickMat[i][j].power.type = &paddleShoot; break;
        case 18: brickMat[i][j].power.type = &splitBall; break;
        case 19: brickMat[i][j].power.type = &eightBall; break;
        case 20: brickMat[i][j].power.type = &fallingBricks; break;
        default: brickMat[i][j].power.type = &extraLife;
    };
    brickMat[i][j].power.active = 1;
};