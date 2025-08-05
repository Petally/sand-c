// Falling sand simulation
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>

#define FRAMERATE 165
#define WORLD_WIDTH 480
#define WORLD_HEIGHT 360
#define WINDOW_SCALING 2

enum ParticleType {
    EMPTY,
    SAND,
    WALL,
    WATER,
    IMPENETRABLE_WALL
};

enum ParticleType grid[WORLD_WIDTH][WORLD_HEIGHT] = { EMPTY };

void InitGrid(void)
{
    // Add walls
    for (int x = 0; x < WORLD_WIDTH; x++) {
        grid[x][0] = IMPENETRABLE_WALL;
        grid[x][WORLD_HEIGHT - 1] = IMPENETRABLE_WALL;
    }

    for (int y = 0; y < WORLD_HEIGHT; y++) {
        grid[0][y] = IMPENETRABLE_WALL;
        grid[WORLD_WIDTH - 1][y] = IMPENETRABLE_WALL;
    }
}

Vector2 MouseToWorldSpace(void)
{
    Vector2 worldPos = GetMousePosition();
    // Scale down (for some reason Vector2Scale doesn't work)
    worldPos = (Vector2){ .x = worldPos.x / WINDOW_SCALING, .y = worldPos.y / WINDOW_SCALING };
    // Bounds
    worldPos = Vector2Clamp(worldPos, (Vector2){ 0, 0 }, (Vector2){ WORLD_WIDTH - 1, WORLD_HEIGHT - 1 });
    return worldPos;
}

// Sets a particle in the grid
void SetParticle(int x, int y, enum ParticleType particleType)
{
    if (grid[x][y] == IMPENETRABLE_WALL) return;
    if (x < 0 || x > WORLD_WIDTH - 1) return;
    if (y < 0 || y > WORLD_HEIGHT - 1) return;

    grid[x][y] = particleType;
}

void SwapParticles(int x1, int y1, int x2, int y2)
{
    enum ParticleType temp = grid[x1][y1];
    SetParticle(x1, y1, grid[x2][y2]);
    SetParticle(x2, y2, temp);
}

void SetParticleRectangle(int centerX, int centerY, int size, enum ParticleType particleType)
{
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            SetParticle(x + centerX - size/2, y + centerY - size/2, particleType);
        }
    }
}

void UpdateSand(int x, int y)
{
    if (grid[x][y + 1] == EMPTY) {
        SwapParticles(x, y, x, y + 1);
    }
    else if (grid[x - 1][y + 1] == EMPTY) {
        SwapParticles(x, y, x - 1, y + 1);
    }
    else if (grid[x + 1][y + 1] == EMPTY) {
        SwapParticles(x, y, x + 1, y + 1);
    }
}

void UpdateWater(int x, int y)
{
    if (grid[x][y + 1] == EMPTY) {
        SwapParticles(x, y, x, y + 1);
    }
    else if (grid[x - 1][y] == EMPTY) {
        SwapParticles(x, y, x - 1, y);
    }
    else if (grid[x + 1][y] == EMPTY) {
        SwapParticles(x, y, x + 1, y);
    }
    else if (grid[x - 1][y + 1] == EMPTY) {
        SwapParticles(x, y, x - 1, y + 1);
    }
    else if (grid[x + 1][y + 1] == EMPTY) {
        SwapParticles(x, y, x + 1, y + 1);
    }
}

void UpdateWorld(void)
{
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = WORLD_HEIGHT; y > 0; y--) {
            switch (grid[x][y]) {
                case SAND:
                    UpdateSand(x, y);
                    break;
                case WATER:
                    UpdateWater(x, y);
                    break;
                default:
                    break;
            }
        }
    }
}

enum ParticleType selectedParticleType = SAND;
void HandleParticleTypeSelectInput()
{
    if (IsKeyPressed('1'))
        selectedParticleType = SAND;
    if (IsKeyPressed('2'))
        selectedParticleType = WATER;
    if (IsKeyPressed('3'))
        selectedParticleType = WALL;
}

int brushSize = 9;
void HandleDrawInput(void)
{
    Vector2 mousePos = MouseToWorldSpace();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        //SetParticle(mousePos.x, mousePos.y, SAND);
        SetParticleRectangle(mousePos.x, mousePos.y, brushSize, selectedParticleType);
    }
}

void HandleBrushSizeInput(void)
{
    if (IsKeyPressed('='))
        brushSize = fmin(brushSize + 1, 15);
    if (IsKeyPressed('-'))
        brushSize = fmax(brushSize - 1, 1);
}

void HandleInput(void)
{
    HandleParticleTypeSelectInput();
    HandleDrawInput();
    HandleBrushSizeInput();
}

// Draws the grid to the screen
void DrawWorld(void)
{
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Color particleColor;
            switch (grid[x][y]) {
                case SAND:
                    particleColor = YELLOW;
                    break;
                case IMPENETRABLE_WALL:
                    particleColor = GRAY;
                    break;
                case WALL:
                    particleColor = GRAY;
                    break;
                case WATER:
                    particleColor = BLUE;
                    break;
                default:
                    particleColor = BLACK;
                    break;
            }
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

    InitGrid();

    while (!WindowShouldClose())
    {
        // Update
        UpdateWorld();
        HandleInput();
        // Draw
        Draw(worldSpaceCamera, screenSpaceCamera, &target, sourceRec, destRec, origin);
    }

    CloseWindow();

    return 0;
}