#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

#define GRAVITY 900
#define JUMP_FORCE -300

#define PIPE_SPEED 200
#define PIPE_WIDTH 60
#define PIPE_GAP 140
#define NUM_PIPES 3
#define PIPE_DISTANCE 300

typedef struct {
    float y;
    float velocity;
} Bird;

typedef struct {
    float x;
    float gapY;
    bool scored;
} Pipe;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird - raylib");
    SetTargetFPS(60);

    Bird bird = { SCREEN_HEIGHT / 2, 0 };

    Pipe pipes[NUM_PIPES];
    for (int i = 0; i < NUM_PIPES; i++)
    {
        pipes[i].x = SCREEN_WIDTH + i * PIPE_DISTANCE;
        pipes[i].gapY = GetRandomValue(80, SCREEN_HEIGHT - PIPE_GAP - 80);
        pipes[i].scored = false;
    }

    int score = 0;
    int highscore = 0;
    bool gameOver = false;

    FILE *file = fopen("highscore.txt", "r");
    if (file != NULL)
    {
        fscanf(file, "%d", &highscore);
        fclose(file);
    }

    while (!WindowShouldClose())
    {
        // ---------- UPDATE ----------
        if (!gameOver)
        {
            if (IsKeyPressed(KEY_SPACE))
                bird.velocity = JUMP_FORCE;

            bird.velocity += GRAVITY * GetFrameTime();
            bird.y += bird.velocity * GetFrameTime();

            for (int i = 0; i < NUM_PIPES; i++)
            {
                pipes[i].x -= PIPE_SPEED * GetFrameTime();

                // Zliczanie punktów
                if (!pipes[i].scored && pipes[i].x + PIPE_WIDTH < 100)
                {
                    score++;
                    pipes[i].scored = true;
                    if (score > highscore) highscore = score;
                }

                // Reset rury po wyjœciu z ekranu
                if (pipes[i].x + PIPE_WIDTH < 0)
                {
                    pipes[i].x = SCREEN_WIDTH + (NUM_PIPES - 1) * PIPE_DISTANCE;
                    pipes[i].gapY = GetRandomValue(80, SCREEN_HEIGHT - PIPE_GAP - 80);
                    pipes[i].scored = false;
                }
            }

            // kolizje z ekranem
            if (bird.y < 0 || bird.y > SCREEN_HEIGHT)
                gameOver = true;

            Rectangle birdRec = { 100, bird.y, 30, 30 };
            for (int i = 0; i < NUM_PIPES; i++)
            {
                Rectangle topPipe = { pipes[i].x, 0, PIPE_WIDTH, pipes[i].gapY };
                Rectangle bottomPipe = { pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT };
                if (CheckCollisionRecs(birdRec, topPipe) || CheckCollisionRecs(birdRec, bottomPipe))
                    gameOver = true;
            }
        }
        else
        {
            if (IsKeyPressed(KEY_R))
            {
                bird.y = SCREEN_HEIGHT / 2;
                bird.velocity = 0;
                score = 0;
                gameOver = false;
                for (int i = 0; i < NUM_PIPES; i++)
                {
                    pipes[i].x = SCREEN_WIDTH + i * PIPE_DISTANCE;
                    pipes[i].gapY = GetRandomValue(80, SCREEN_HEIGHT - PIPE_GAP - 80);
                    pipes[i].scored = false;
                }
            }
        }

        // ---------- DRAW ----------
        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawRectangle(100, bird.y, 30, 30, YELLOW);

        for (int i = 0; i < NUM_PIPES; i++)
        {
            DrawRectangle(pipes[i].x, 0, PIPE_WIDTH, pipes[i].gapY, GREEN);
            DrawRectangle(pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT, GREEN);
        }

        DrawText(TextFormat("Score: %i", score), 10, 10, 20, BLACK);
        DrawText(TextFormat("Highscore: %i", highscore), 10, 40, 20, DARKGRAY);


        if (gameOver)
        {
            DrawText("GAME OVER", 300, 180, 40, RED);
            DrawText("Press R to restart", 280, 230, 20, BLACK);
        }

        EndDrawing();
    }

    FILE *fileOut = fopen("highscore.txt", "w");
    if (fileOut != NULL)
    {
        fprintf(fileOut, "%d", highscore);
        fclose(fileOut);
    }

    CloseWindow();
    return 0;
}
