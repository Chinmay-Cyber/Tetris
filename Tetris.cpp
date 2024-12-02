#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>

// Constants
const int screenWidth = 400;
const int screenHeight = 800;
const int gridWidth = 10;
const int gridHeight = 20;
const int blockSize = 40;

// Tetromino shapes
const int shapes[7][4][4] = {
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // I
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Z
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // O
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // L
    {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}  // J
};

// Colors for each tetromino
const Color tetrominoColors[7] = {
    SKYBLUE, RED, GREEN, PURPLE, YELLOW, ORANGE, BLUE
};

// Game state
std::vector<std::vector<int>> grid(gridHeight, std::vector<int>(gridWidth, 0));
std::vector<std::vector<Color>> gridColors(gridHeight, std::vector<Color>(gridWidth, BLANK));
int currentTetromino[4][4];
Color currentColor;
int posX = 3, posY = 0;
int score = 0;
float timer = 0;
float delay = 0.5f;

// Helper functions
bool CheckCollision(int offsetX, int offsetY) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentTetromino[y][x]) {
                int newX = posX + x + offsetX;
                int newY = posY + y + offsetY;
                if (newX < 0 || newX >= gridWidth || newY >= gridHeight || (newY >= 0 && grid[newY][newX])) {
                    return true;
                }
            }
        }
    }
    return false;
}

void LockTetromino() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentTetromino[y][x]) {
                grid[posY + y][posX + x] = 1;
                gridColors[posY + y][posX + x] = currentColor;
            }
        }
    }
}

void ClearLines() {
    for (int y = gridHeight - 1; y >= 0; y--) {
        bool fullLine = true;
        for (int x = 0; x < gridWidth; x++) {
            if (!grid[y][x]) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            grid.erase(grid.begin() + y);
            grid.insert(grid.begin(), std::vector<int>(gridWidth, 0));
            gridColors.erase(gridColors.begin() + y);
            gridColors.insert(gridColors.begin(), std::vector<Color>(gridWidth, BLANK));
            score += 100;
            y++; // Check the same line again
        }
    }
}

void RotateTetromino() {
    int rotated[4][4] = { 0 };
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            rotated[x][3 - y] = currentTetromino[y][x];
        }
    }
    std::copy(&rotated[0][0], &rotated[0][0] + 16, &currentTetromino[0][0]);
    if (CheckCollision(0, 0)) {
        std::copy(&rotated[0][0], &rotated[0][0] + 16, &currentTetromino[0][0]); // Undo rotation
    }
}

void SpawnTetromino() {
    int index = rand() % 7;
    std::copy(&shapes[index][0][0], &shapes[index][0][0] + 16, &currentTetromino[0][0]);
    currentColor = tetrominoColors[index];
    posX = 3;
    posY = 0;
    if (CheckCollision(0, 0)) {
        // Game over
        grid.assign(gridHeight, std::vector<int>(gridWidth, 0));
        gridColors.assign(gridHeight, std::vector<Color>(gridWidth, BLANK));
        score = 0;
    }
}

// Drawing functions
void DrawGrid() {
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            if (grid[y][x]) {
                DrawRectangle(x * blockSize, y * blockSize, blockSize - 2, blockSize - 2, gridColors[y][x]);
            }
        }
    }
}

void DrawTetromino() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentTetromino[y][x]) {
                DrawRectangle((posX + x) * blockSize, (posY + y) * blockSize, blockSize - 2, blockSize - 2, currentColor);
            }
        }
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Tetris - Colorful Blocks");
    SetTargetFPS(60);
    srand(time(nullptr));

    SpawnTetromino();

    while (!WindowShouldClose()) {
        // Input handling
        if (IsKeyPressed(KEY_LEFT) && !CheckCollision(-1, 0)) posX--;
        if (IsKeyPressed(KEY_RIGHT) && !CheckCollision(1, 0)) posX++;
        if (IsKeyPressed(KEY_DOWN) && !CheckCollision(0, 1)) posY++;
        if (IsKeyPressed(KEY_UP)) RotateTetromino();

        // Update
        timer += GetFrameTime();
        if (timer > delay) {
            timer = 0;
            if (!CheckCollision(0, 1)) {
                posY++;
            }
            else {
                LockTetromino();
                ClearLines();
                SpawnTetromino();
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawGrid();
        DrawTetromino();

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}