// Falling sand simulation
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define FRAMERATE 165
#define WORLD_WIDTH 480
#define WORLD_HEIGHT 360
#define WINDOW_SCALING 2

enum ParticleType {
    EMPTY,
    SAND,
    WALL,
    WATER,
};

enum ParticleType grid[WORLD_WIDTH][WORLD_HEIGHT] = { EMPTY };

Vector2 MouseToWorldSpace()
{
    Vector2 worldPos = GetMousePosition();
    // Scale down (for some reason Vector2Scale doesn't work)
    worldPos = (Vector2){ .x = worldPos.x / WINDOW_SCALING, .y = worldPos.y / WINDOW_SCALING };
    // Bounds
    worldPos = Vector2Clamp(worldPos, (Vector2){ 0, 0 }, (Vector2){ WORLD_WIDTH - 1, WORLD_HEIGHT - 1 });
    return worldPos;
}

// Sets a particle in the grid
// Do not pass out of bounds values
void SetParticle(int x, int y, enum ParticleType particleType)
{
    grid[x][y] = particleType;
}

void HandleInput(void)
{
    Vector2 mousePos = MouseToWorldSpace();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        SetParticle(mousePos.x, mousePos.y, SAND);
    }
}

// Draws the grid to the screen
void DrawWorld(void)
{
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Color particleColor = BLACK;
            if (grid[x][y] == SAND)
                particleColor = YELLOW;
            DrawPixel(x, y, particleColor);
        }
    }
}

void Draw(Camera2D worldSpaceCamera, Camera2D screenSpaceCamera, RenderTexture2D *target, Rectangle sourceRec, Rectangle destRec, Vector2 origin)
{
    BeginTextureMode(*target);
        ClearBackground(RAYWHITE);

        BeginMode2D(worldSpaceCamera);
            DrawWorld();
        EndMode2D();
    EndTextureMode();

    BeginDrawing();
        ClearBackground(RED);

        BeginMode2D(screenSpaceCamera);
            DrawTexturePro(target->texture, sourceRec, destRec, origin, 0.0f, WHITE);
        EndMode2D();
    EndDrawing();
}

int main(void)
{
    const int screenWidth = WORLD_WIDTH * WINDOW_SCALING;
    const int screenHeight = WORLD_HEIGHT * WINDOW_SCALING;

    InitWindow(screenWidth, screenHeight, "dSand 2");

    SetTargetFPS(FRAMERATE);

    Camera2D worldSpaceCamera = { 0 };  // Game world camera
    worldSpaceCamera.zoom = 1.0f;
    worldSpaceCamera.target = (Vector2){ 0, 0 };

    Camera2D screenSpaceCamera = { 0 }; // Smoothing camera
    screenSpaceCamera.zoom = 1.0f;
    screenSpaceCamera.target = (Vector2){ 0, 0 };

    RenderTexture2D target = LoadRenderTexture(WORLD_WIDTH, WORLD_HEIGHT); // This is where we'll be drawing everything

    // The target's height is flipped (in the source Rectangle), due to OpenGL reasons
    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
    Rectangle destRec = { 0, 0, screenWidth, screenHeight };

    Vector2 origin = { 0.0f, 0.0f };

    while (!WindowShouldClose())
    {
        // Update
        HandleInput();
        // Draw
        Draw(worldSpaceCamera, screenSpaceCamera, &target, sourceRec, destRec, origin);
    }

    CloseWindow();

    return 0;
}