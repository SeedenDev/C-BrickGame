#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "helper.h"

int viewportWidth = 30;
int viewportHeight = 15;
int bricksHeight = 10;
int platformSize = 2;


typedef struct {
    int x;
    int y;
    bool destroyed;
} Brick;

typedef struct {
    int x;
    int y;
    bool goingUp;
    bool goingLeft;
    bool reverseYVelocity;
} Ball;

typedef struct {
    int x;
    int velocity;
} Platform;

int toVelocity(const bool direction) {
    return direction ? -1 : 1;
}

int getIndexForPosition(const int x, const int y) {
    return (y-1)*viewportWidth + x;
}
bool brickExistAtPos(const int x, const int y, const Brick* bricks) {
    const int index = getIndexForPosition(x, y);
    if (index < 0 || index>=viewportWidth*bricksHeight) return 0;
    return !bricks[index].destroyed;
}

bool checkBallCollision(const Ball* ball, const Brick* bricks, Brick** collidedBrick) {
    const int ballX = ball->x;
    const int ballY = ball->y;
    const bool up = ball->goingUp;
    const bool left = ball->goingLeft;

    // First check top, then if LEFT: left then top-left / RIGHT: right then top-right
    if (up) {
        // TOP
        if (brickExistAtPos(ballX, ballY-1, bricks)) {
            *collidedBrick = &bricks[getIndexForPosition(ballX, ballY-1)];
            return 1;
        }
        if (left) {
            // LEFT
            if (brickExistAtPos(ballX-1, ballY, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX-1, ballY)];
                return 1;
            }
            // TOP-LEFT
            if (brickExistAtPos(ballX-1, ballY-1, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX-1, ballY-1)];
                return 1;
            }
        }
        else {
            // RIGHT
            if (brickExistAtPos(ballX+1, ballY, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX+1, ballY)];
                return 1;
            }
            // TOP-RIGHT
            if (brickExistAtPos(ballX+1, ballY-1, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX+1, ballY-1)];
                return 1;
            }
        }
    }
    // Else check bottom, then if LEFT: also left then bt-left / Right= right also + bt-right ofc
    else {
        // BOTTOM
        if (brickExistAtPos(ballX, ballY+1, bricks)) {
            *collidedBrick = &bricks[getIndexForPosition(ballX, ballY-1)];
            return 1;
        }
        if (left) {
            // LEFT
            if (brickExistAtPos(ballX-1, ballY, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX-1, ballY)];
                return 1;
            }
            // BOTTOM-LEFT
            if (brickExistAtPos(ballX-1, ballY+1, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX-1, ballY-1)];
                return 1;
            }
        }
        else {
            // RIGHT
            if (brickExistAtPos(ballX+1, ballY, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX+1, ballY)];
                return 1;
            }
            // BOTTOM-RIGHT
            if (brickExistAtPos(ballX+1, ballY+1, bricks)) {
                *collidedBrick = &bricks[getIndexForPosition(ballX+1, ballY-1)];
                return 1;
            }
        }
    }
    return 0;
}

bool ballCollideWithBrick(const Ball* ball, const Brick* brick, bool* isSideBrick){
    if (brick->destroyed) return 0;
    // Check for side bricks
    if (ball->y==brick->y && ((ball->goingLeft && ball->x-1==brick->x) || (!ball->goingLeft && ball->x+1==brick->x))) {
        *isSideBrick = 1;
        return 1;
    }
    // Check for top/bottom brick
    if ((ball->goingUp && ball->y==brick->y+1) || (!ball->goingUp && ball->y==brick->y-1)) {
        if (ball->x==brick->x) return 1;
        //if ((ball->goingLeft && ball->x-1==brick->x) || (!ball->goingLeft && ball->x+1==brick->x)) return 1;
    }
    return 0;
}
bool ballCollideWithPlatform(const Ball* ball, const Platform* platform){
    return !ball->goingUp && ball->x >=platform->x && ball->x <=platform->x+platformSize && ball->y==viewportHeight-2;
}

void moveBall(Ball* ball, const Brick* bricks){
    int newX = ball->x;
    int newY = ball->y;

    if(!brickExistAtPos(ball->x+toVelocity(ball->goingLeft), ball->y+toVelocity(ball->goingUp), bricks)) {
        newX += toVelocity(ball->goingLeft);
        newY += toVelocity(ball->goingUp);
    }

    /*if (!brickExistAtPos(ball->x+toVelocity(ball->goingLeft), ball->y, bricks) && !brickExistAtPos(ball->x, ball->y+toVelocity(ball->goingUp), bricks)
        && !brickExistAtPos(ball->x+toVelocity(ball->goingLeft), ball->y+toVelocity(ball->goingUp), bricks)) {

        newX += toVelocity(ball->goingLeft);
        newY += toVelocity(ball->goingUp);
    }*/

    // if collide with brick just go up and notify the collision + no x-move + reverse y-velocity
    //then check x-sides collisions, don't change the y-velocity but the x-velocity
    // if collide with brick, check beforehand if there is a brick above/below (according to the velocity) = if there is a brick on the side
    // and if yes, cancel the move and bounce? and notify the collision if the side. And if no side, then just notify collision with the diagonal



    ball->x += toVelocity(ball->goingLeft);
    ball->y += toVelocity(ball->goingUp);
    if (ball->reverseYVelocity) {
        ball->goingUp = !ball->goingUp;
        ball->reverseYVelocity = 0;
    }
    // ball->x = newX;
    // ball->y = newY;
    if(ball->x==1 || ball->x==viewportWidth) ball->goingLeft = !ball->goingLeft;
    if(ball->y==1) ball->goingUp = 0;
}

void movePlatformLeft(Platform* platform){
    platform->x--;
    platform->velocity = -1;
}
void movePlatformRight(Platform* platform){
    platform->x++;
    platform->velocity = 1;
}

void startAndReset(Platform* platform, Ball* ball, Brick* bricks, const size_t bricksCount) {
    platform->x = viewportWidth/2+platformSize/2-2;
    platform->velocity = 0;
    ball->x = platform->x+platformSize/2;
    ball->y = viewportHeight-2;
    ball->goingUp = 1;
    ball-> goingLeft = rand()%2;
    for(int i=0; i<bricksCount; i++){
        Brick brick = {i%30+1, i/viewportWidth+1, 0};
        bricks[i] = brick;
    }
}

void printGame(const Ball ball, const Platform platform, const Brick* bricks) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    //ClearScreen();
    for(size_t i = 0; i<=viewportHeight; i++){
        for(size_t j = 0; j<=viewportWidth+1; j++){
            if(i==viewportHeight || i==0) printf("-");
            else if(j==0 || j==viewportWidth+1) printf("|");
            else if(ball.x==j && ball.y==i) printf("o");
            else if(i<=bricksHeight){
                const Brick brick = bricks[(i-1)*viewportWidth+(j-1)];
                if(brick.destroyed) printf(" ");
                else printf("#");
            }
            else if(i==viewportHeight-1 && j>=platform.x && j<=platform.x+platformSize) printf("-");
            else printf(" ");
        }
        printf("\n");
    }
    printf("Ballpos: x%i y%i => %i\n", ball.x, ball.y, (ball.y-1)*viewportWidth+ball.x);
}

int main()
{
    bool playing = 1;
    srand(time(NULL));

    Platform platform;
    Ball ball;
    const size_t bricksCount = viewportWidth*bricksHeight;
    Brick bricks[bricksCount];

    startAndReset(&platform, &ball, bricks, bricksCount);
    int score = 0;
    printGame(ball, platform, bricks);

    while(playing){
        printGame(ball, platform, bricks);
        /*for(int i=bricksCount-1; i>=0; i--){
            Brick* brick = &bricks[i];
            bool isSideBrick = 0;
            if (ballCollideWithBrick(&ball, brick, &isSideBrick)) {
                score++;
                brick->destroyed = 1;
                if (!isSideBrick) ball.goingUp = 0;
                // Chances of changing x velocity
                if (rand()%30==0) ball.goingLeft = !ball.goingLeft;
                break;
            }
        }*/

        /* Movements */
        // pb : it stops the loop but anyway the way to do it would be to have a true key listener on another thread
        char input = getchar();
        switch (input) {
            case 'q':
                movePlatformLeft(&platform);
                break;
            case 'd':
                movePlatformRight(&platform);
                break;
            default:
                platform.velocity = 0;
                break;
        }

        // Escape the new line character added after a command above
        if (input=='\n') moveBall(&ball, bricks);

        /* Bricks collision check */
        Brick* collidedBrick;
        if (checkBallCollision(&ball, bricks, &collidedBrick)) {
            score++;
            collidedBrick->destroyed = 1;
            // Side = just reverse the x velocity and keep the y's one
            if (collidedBrick->y==ball.y) ball.goingLeft = !ball.goingLeft;
            else {
                // Reverse y velocity and random the x's one (must more chances to keep the same)
                //ball.goingUp = !ball.goingUp;
                ball.reverseYVelocity = 1;
                if (rand()%35==0) ball.goingLeft = !ball.goingLeft;
            }
        }

        /* Platform collision check */
        if(ballCollideWithPlatform(&ball, &platform)){
            ball.goingUp = 1;
            switch (platform.velocity) {
                case 1:
                    ball.goingLeft = 0;
                    break;
                case 2:
                    ball.goingLeft = 1;
                    break;
                default: ball.goingLeft = !ball.goingLeft;
            }
        }

        /* Lose detection */
        if (ball.y==viewportHeight-1) {
            printf("You hit the ground! Score: %i\nPlay again? (Y/N)", score);
            char input = getchar();
            if(input=='y' || input=='Y') {
                startAndReset(&platform, &ball, &bricks, bricksCount);
                score = 0;
            }else{
                playing = 0;
                break;
            }
        }
    }
    return 0;
}