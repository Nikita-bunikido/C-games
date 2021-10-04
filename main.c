#include "include\raylib.h"
#include "stdlib.h"

#define w               800
#define h               800

#define tile            50
#define fieldoffset     100
#define fieldw          16
#define fieldh          6

bool run_ball = 0;
Texture2D ballt, rackett;
Texture2D block1, block2;

Sound fxblock, fxwall, fxgameover;
char score_str[100];
long score = 0;

struct {
    Vector2 pos;
    int width;
    int height;
    int speed;
} racket;

struct {
    Vector2 pos;
    Vector2 d;
    int r;
} ball;

typedef bool map[fieldh][fieldw];

float module(float _x){
    return (_x > 0) ? _x : -_x;
}

void initMap(map Map){
    int i, j;
    for(i = 0; i < fieldh; i++)
        for(j = 0; j < fieldw; j++)
            Map[i][j] = true;
}

void initRacket(int _w, int _h, int _speed){
    racket.width = _w;
    racket.height = _h;
    racket.speed = _speed;

    racket.pos = (Vector2){(w - racket.width) / 2, h - racket.height - 10};
}

void initBall(int _r, float dx, float dy){
    ball.r = _r;
    ball.d = (Vector2){dx, dy};
}

void moveRacket(int _x){
    racket.pos.x = _x;

    if (racket.pos.x < 10)
        racket.pos.x = 10;
    if (racket.pos.x + racket.width > w - 10)
        racket.pos.x = w - 10 - racket.width;
}

void moveBall(int _x, int _y){
    ball.pos.x = _x;
    ball.pos.y = _y;
}

void userInput(int _left, int _right){
    if (IsKeyDown(_left)) moveRacket(racket.pos.x - racket.speed);
    if (IsKeyDown(_right)) moveRacket(racket.pos.x + racket.speed);
    if (IsKeyDown(KEY_SPACE)) run_ball = true;
}

void Show(map Map){
    //DrawRectangle(racket.pos.x, racket.pos.y, racket.width, racket.height, PURPLE); // racket
    //DrawCircle(ball.pos.x, ball.pos.y, ball.r, MAROON);                             // ball

    DrawTexture(ballt, ball.pos.x - ball.r, ball.pos.y - ball.r, WHITE);   // racket
    DrawTexture(rackett, racket.pos.x, racket.pos.y - 20,  WHITE);         // ball

    itoa(score, score_str, 10);
    DrawText(score_str, 170, 10, 40, (Color){143, 166, 136, 255});
    DrawText("SCORE: ", 10, 10, 40, (Color){143, 166, 136, 255});

    for(int i = 0; i < fieldh; i++)
        for(int j = 0; j < fieldw; j++)
            if (Map[i][j] == true)
                DrawTexture(!((i + j) % 2) ? block1 : block2, j * tile, i * tile + fieldoffset, WHITE);
                //DrawRectangle(j * tile, i * tile + fieldoffset, tile,tile, !((i + j) % 2) ? (Color){100, 100, 100, 255} : (Color){200, 200, 200, 255});

}

void autoMoveBall(map Map){
    /* moving */
    moveBall(ball.pos.x + ball.d.x, ball.pos.y + ball.d.y);

    /* walls collision*/
    if (ball.pos.y < ball.r) { ball.d.y *= -1; PlaySound(fxwall); }
    if (ball.pos.x < ball.r || ball.pos.x > w - ball.r) { ball.d.x *= -1; PlaySound(fxwall); }

    /* racket collision*/
    if (ball.pos.x > racket.pos.x && ball.pos.x < racket.pos.x + racket.width && ball.pos.y + ball.r >= racket.pos.y){
        ball.d.y *= -1;
        PlaySound(fxwall);
    }

    /* game over check */
    if (ball.pos.y > racket.pos.y - ball.r) { run_ball = false; PlaySound(fxgameover); }

    for(int i = 0; i < fieldh; i++)
        for(int j = 0; j < fieldw; j++)
            if (Map[i][j] && (ball.pos.x - ball.r < j * tile + tile) && (ball.pos.x + ball.r > j * tile)
                && (ball.pos.y - ball.r < i * tile + tile + fieldoffset) && (ball.pos.y + ball.r > i * tile + fieldoffset)){


                Map[i][j] = !Map[i][j];
                if (module(ball.d.y) > module(ball.d.x)) ball.d.y *= -1;
                else ball.d.x *= -1;
                PlaySound(fxblock);
                ++score;
                return;
            }
}



int main(void){
    InitWindow(w, h, "Arcanoid game");
    InitAudioDevice();


    ballt = LoadTexture("data\\ball.png");
    rackett = LoadTexture("data\\racket.png");
    block1 = LoadTexture("data\\block1.png");
    block2 = LoadTexture("data\\block2.png");

    fxwall = LoadSound("sounds\\wallhit.wav");
    fxblock = LoadSound("sounds\\blockhit.wav");
    fxgameover = LoadSound("sounds\\gameover.wav");

    map Map1;
    initRacket(200, 30, 8);
    initBall(23, 3, -5);
    initMap(Map1);
    SetTargetFPS(60);

    while (!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(RAYWHITE);
            Show(Map1);
        EndDrawing();

        
        if (run_ball == true){
            autoMoveBall(Map1);
        }
        else {
            moveBall(racket.pos.x + racket.width / 2, racket.pos.y - ball.r - 5);
        }
        userInput(KEY_LEFT, KEY_RIGHT);
    }

    CloseWindow();

    return 0;
}
