// Falling sand simulation
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FRAMERATE 60
#define WORLD_WIDTH 512
#define WORLD_HEIGHT 512
#define WINDOW_SCALING 1

enum ParticleType {
    EMPTY,
    SAND,
    WALL,
    WATER,
    IMPENETRABLE_WALL,
    OIL,
    FIRE,
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
    if (grid[x][y + 1] == EMPTY || grid[x][y + 1] == WATER) {
        SwapParticles(x, y, x, y + 1);
    }
    else if (grid[x - 1][y + 1] == EMPTY || grid[x - 1][y + 1] == WATER) {
        SwapParticles(x, y, x - 1, y + 1);
    }
    else if (grid[x + 1][y + 1] == EMPTY || grid[x + 1][y + 1] == WATER) {
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

void UpdateOil(int x, int y)
{
    if (grid[x][y + 1] == EMPTY || grid[x][y + 1] == WATER) {
        SwapParticles(x, y, x, y + 1);
    }
    else if (grid[x - 1][y] == EMPTY || grid[x - 1][y] == WATER) {
        SwapParticles(x, y, x - 1, y);
    }
    else if (grid[x + 1][y] == EMPTY || grid[x + 1][y] == WATER) {
        SwapParticles(x, y, x + 1, y);
    }
    else if (grid[x - 1][y + 1] == EMPTY || grid[x - 1][y + 1] == WATER) {
        SwapParticles(x, y, x - 1, y + 1);
    }
    else if (grid[x + 1][y + 1] == EMPTY || grid[x + 1][y + 1] == WATER) {
        SwapParticles(x, y, x + 1, y + 1);
    }
}

void UpdateFire(int x, int y)
{
    int extinguishChance = 20;
    int shouldExtingusih = (rand() % 100) < extinguishChance;
    if (shouldExtingusih == 1)
        SetParticle(x, y, EMPTY);

    int catchChance = 50;
    if (grid[x][y + 1] == OIL) {
        int shouldCatch = (rand() % 100) < catchChance;
        if (shouldCatch == 1)
            SetParticle(x, y + 1, FIRE);
    }
    if (grid[x - 1][y] == OIL) {
        int shouldCatch = (rand() % 100) < catchChance;
        if (shouldCatch == 1)
            SetParticle(x - 1, y, FIRE);
    }
    if (grid[x + 1][y] == OIL) {
        int shouldCatch = (rand() % 100) < catchChance;
        if (shouldCatch == 1)
            SetParticle(x + 1, y , FIRE);
    }
    if (grid[x][y - 1] == OIL) {
        int shouldCatch = (rand() % 100) < catchChance;
        if (shouldCatch == 1)
            SetParticle(x, y - 1, FIRE);
    }
}

void UpdateWorld(void)
{
    for (int row = WORLD_HEIGHT - 1; row > 0; row--) {
        int leftToRight = (rand() % 2) > 0;

        for (int i = 0; i < WORLD_WIDTH; i++) {
            int columnOffset = leftToRight ? i : -i - 1 + WORLD_WIDTH;
            switch (grid[columnOffset][row]) {
                case SAND:
                    UpdateSand(columnOffset, row);
                    break;
                case WATER:
                    UpdateWater(columnOffset, row);
                    break;
                case FIRE:
                    UpdateFire(columnOffset, row);
                    break;
                case OIL:
                    UpdateOil(columnOffset, row);
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
    if (IsKeyPressed('4'))
        selectedParticleType = EMPTY;
    if (IsKeyPressed('5'))
        selectedParticleType = FIRE;
    if (IsKeyPressed('6'))
        selectedParticleType = OIL;
}

int brushSize = 32;
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
        brushSize = fmin(brushSize + 2, 32);
    if (IsKeyPressed('-'))
        brushSize = fmax(brushSize - 2, 1);
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
                case FIRE:
                    particleColor = RED;
                    break;
                case OIL:
                    particleColor = PURPLE;
                    break;
                default:
                    particleColor = BLACK;
                    break;
            }
            DrawPixel(x, y, particleColor);
        }
    }
}

void Draw(Camera2D worldSpaceCamera, Camera2D screenSpaceCamera, 
    RenderTexture2D *target, 
    Rectangle sourceRec, Rectangle destRec)
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
            DrawTexturePro(target->texture, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
            DrawFPS(0, 0);
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

    InitGrid();

    while (!WindowShouldClose())
    {
        // Update
        UpdateWorld();
        HandleInput();
        // Draw
        Draw(worldSpaceCamera, screenSpaceCamera, &target, sourceRec, destRec);
    }

    CloseWindow();

    return 0;
}