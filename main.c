// Falling sand simulation
#include "raylib.h"

#define FRAMERATE 165
#define CANVAS_WIDTH 480
#define CANVAS_HEIGHT 360
#define WINDOW_SCALING 2

int main(void)
{
    const int screenWidth = CANVAS_WIDTH * WINDOW_SCALING;
    const int screenHeight = CANVAS_HEIGHT * WINDOW_SCALING;

    InitWindow(screenWidth, screenHeight, "dSand 2");

    SetTargetFPS(FRAMERATE);

    while (!WindowShouldClose())
    {
        // Update
       
        // Draw
        BeginDrawing();
            ClearBackground(WHITE);
            
        EndDrawing();
    }

    CloseWindow();

    return 0;
}