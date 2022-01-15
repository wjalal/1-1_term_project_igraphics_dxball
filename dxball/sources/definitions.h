typedef enum _BrickType {
    NONE, NORMAL, EXPLODING, INVISIBLE, 
    METAL, ROCK_1, ROCK_2, ROCK_3
} BrickType;

typedef enum _PaddleType {
    P_NORMAL, P_GRAB, P_SHOOT, P_GRABSHOOT
} PaddleType;

typedef enum _BallType {
    T_NORMAL, T_FIRE, T_THRU
} BallType;

// typedef enum _BallSize {
//     S_NORMAL, S_SMALL, S_LARGE
// } BallSize;

typedef struct _Point {
    double x, y;
} Point;

typedef struct _Color {
    double R, G, B;
} Color;

typedef struct _PowerUp {
    unsigned int active : 1;
    double x, y, vx, vy;
    void (*type)(void);
    unsigned int image : 7;
} PowerUp;

typedef struct _Brick {
    Point pos;
    BrickType type;
    Color col;
    //char texture[100];
    int textureImg;
    unsigned int shardAnimState : 1;
    double shardVel[10], shardAng[10], 
           shardX[10], shardY[10];
    PowerUp power;
} Brick;

typedef struct _Ball {
    BallType type;
    unsigned int size : 6,
                 trailAnimState : 1;
    double speed, dx, dy, theta;
    Point pos, trails[10];
    int i, j, prevI, prevJ;
} Ball;

typedef struct _MenuBall {
    BallType type;
    unsigned int size : 6,
                 shardAnimState : 1,
                 trailAnimState : 1;
    double speed, dx, dy, theta;
    Point pos, trails[10];
    double shardVel[10], shardAng[10], 
           shardX[10], shardY[10];
} MenuBall;

typedef struct _Paddle {
    int size, lastBullet, ballCount, bottomRow;
    Point pos, spark[20];
    PaddleType type;
    unsigned int resetStatus : 1,
                 sparkAnimStatus : 1,
                 shardAnimState : 1,
                 brickFallState : 1;
    double shardVel[50], shardAng[50], 
           shardX[50], shardY[50];
} Paddle;

typedef struct _Bullet {
    Point pos;
    unsigned int active : 1;
    int i, j, prevI, prevJ;
} Bullet;

typedef struct _Region {
    Point A, B;
} Region;

typedef struct _MenuElement {
    Region region;
    char str[100];
    Color col;
    int n;
    void (*func)(void);
} MenuElement;

struct {
    int fullScreen, 
        gameMusic, 
        menuMusic, 
        bgTexture;
} settings;
