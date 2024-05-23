#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define SPACE 12

//User-defined types
typedef enum GameScreen { Title = 0, GamePlay, Ending} GameScreen;
typedef enum Direction {Up = 0, Down} Direction;

//Global Variables
Rectangle screen, playableBorder, top, bottom, ball, leftRacket, rightRacket;

int rightScore = 0, leftScore = 0, scoreWidth, winner;


void Initialize(void) {
    InitWindow(0, 0, "Ping Pong");
    
    screen = (Rectangle) {0, 0, GetScreenWidth()/2, GetScreenHeight()/2};
    playableBorder = (Rectangle) {SPACE, SPACE, screen.width - (2*SPACE), screen.height - (2*SPACE)};
    top = (Rectangle) {screen.x, screen.y, playableBorder.width, playableBorder.y};
    bottom = (Rectangle) {screen.x, playableBorder.height+SPACE, screen.width, screen.y};
    
    ball = (Rectangle) {5*SPACE, playableBorder.height,SPACE-5, SPACE};
    leftRacket = (Rectangle) {playableBorder.x, playableBorder.height/2, SPACE, 5*SPACE};
    rightRacket = (Rectangle) {playableBorder.width, playableBorder.height/2, SPACE, 5*SPACE};
    scoreWidth = MeasureText("00",60);
    
    SetWindowPosition(screen.width/2, screen.height/2);
    SetWindowSize(screen.width, screen.height);
    SetTargetFPS(60);
}

void serveBall(void) {
    ball.x = playableBorder.width/2;
    ball.y = GetRandomValue(playableBorder.y + 10, playableBorder.height);
}

void moveBall(void)
{
    static int xx = SPACE/2;
    static int yy = SPACE/2;

    // Check collision with rackets and ball has not surpassed rackets.
    if ((CheckCollisionRecs(ball, leftRacket) && ball.x < leftRacket.x + leftRacket.width) ||
        (CheckCollisionRecs(ball, rightRacket) && ball.x > rightRacket.x - rightRacket.width))
        xx = -xx;
    else
        if (CheckCollisionRecs(ball, top) || CheckCollisionRecs(ball, bottom))
            yy = -yy;
        else
        {
            // Score.
            if (ball.x < screen.x)
            {
                ++rightScore;
                serveBall();
            }
            else if (ball.x > screen.width)
            {
                ++leftScore;
                serveBall();
            }
        }

    // Move ball.
    ball.x += xx;
    ball.y += yy;
}

void moveRacket(Rectangle *pRacket, Direction pDir)
{
    int step = (pDir == Up)? -SPACE/2: SPACE/2;

    if ((CheckCollisionRecs(top, *pRacket) && pDir == Up) ||
        (CheckCollisionRecs(bottom, *pRacket) && pDir == Down))
            return;
    pRacket->y += step;
}

int main(void) {
    GameScreen currentScreen = Title;
    Initialize();
    
    Font font1 = LoadFont("resources/font/alpha_beta.png");
    
    while(!WindowShouldClose()) {
        //GameScreens
        switch(currentScreen) {
            case Title:
            {
                if(IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                    currentScreen = GamePlay;
            } break;
            case GamePlay:
            {
                moveBall();
                
                if(IsKeyDown(KEY_Q))
                    moveRacket(&leftRacket, Up);
                else if(IsKeyDown(KEY_A))
                    moveRacket(&leftRacket, Down);
                
                if (IsKeyDown(KEY_I))
                    moveRacket(&rightRacket, Up);
                else if (IsKeyDown(KEY_J))
                    moveRacket(&rightRacket, Down);
                
                if((leftScore >= 5) || (rightScore >= 5))
                {
                    if(abs(leftScore - rightScore) < 2)
                        break;
                    winner = (leftScore > rightScore) ? 1 : 2;
                    rightScore = leftScore = 0;
                    currentScreen = Ending;
                }
            }
            case Ending:
            {
                if(IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                    currentScreen = GamePlay;
            } break;
            default: break;
        }
        //Render
        BeginDrawing();
        switch(currentScreen) {
            case Title:
            {
                ClearBackground(BLACK);
                
                DrawTextEx(font1, "Ping Pong", (Vector2){120, 20}, 130, 2, WHITE);
                DrawTextEx(font1, "Player 1 : Q & A", (Vector2){120, GetScreenHeight()/2 - 25}, 30, 2, WHITE);
                DrawTextEx(font1, "Player 2 : I & J", (Vector2){120, GetScreenHeight()/2 + 25}, 30, 2, WHITE);
                DrawTextEx(font1, "Press ENTER to START", (Vector2){120, 400}, 35, 2, WHITE);
                DrawTextEx(font1, "Made by Nusair", (Vector2){GetScreenWidth()/2.5, 500}, 20, 2, WHITE);
            } break;
            case GamePlay:
            {
                DrawRectangle(screen.x, screen.y, screen.width, screen.height, LIGHTGRAY);
                DrawRectangle(screen.x, playableBorder.y, screen.width, playableBorder.height, BLACK);
                DrawRectangle((screen.width/2) - 5, playableBorder.y, SPACE-2, playableBorder.height, GRAY);
                // Draw score.
                DrawText(TextFormat("%02d", leftScore), (screen.width/2) - 50 - scoreWidth, 50, 40, GRAY);
                DrawText(TextFormat("%02d", rightScore), (screen.width/2) + 50, 50, 40, GRAY);
                // Draw ball.
                DrawCircle(ball.x, ball.y, ball.width, WHITE);
                // Draw rackets.
                DrawRectangle(leftRacket.x, leftRacket.y, leftRacket.width, leftRacket.height, WHITE);
                DrawRectangle(rightRacket.x, rightRacket.y, rightRacket.width, rightRacket.height, WHITE);
            } break;
            case Ending:
            {
                ClearBackground(BLACK);
                
                DrawTextEx(font1,TextFormat("Winner is Player %d", winner), (Vector2){120, 50}, 60, 2, WHITE);
                DrawTextEx(font1,"Press ENTER to PLAY AGAIN", (Vector2){120, 420}, 20, 2, WHITE);
                DrawTextEx(font1,"Press ESCAPE to QUIT", (Vector2){120, 450}, 20, 2, WHITE);
            } break;
            default: break;
            }
            EndDrawing();
        }
        UnloadFont(font1);
        CloseWindow();
        return 0;
    }