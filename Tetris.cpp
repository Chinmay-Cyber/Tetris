#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>


const int screenWidth = 400;
const int screenHeight = 800;
const int gridWidth = 10;
const int gridHeight = 20;
const int blockSize = 40;


const int shapes[7][4][4] = {
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
};

const Color tetrominoColors[7] = {
    SKYBLUE, RED, GREEN, PURPLE, YELLOW, ORANGE, BLUE
};


class Tetromino {
public:
    int shape[4][4];
    Color color;
    int posX, posY;

    Tetromino() : posX(3), posY(0) {
        Spawn();
    }

    void Spawn() {
        int index = rand() % 7;
        std::copy(&shapes[index][0][0], &shapes[index][0][0] + 16, &shape[0][0]);
        color = tetrominoColors[index];
        posX = 3;
        posY = 0;
    }

    void Rotate() {
        int rotated[4][4] = { 0 };
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                rotated[x][3 - y] = shape[y][x];
            }
        }
        std::copy(&rotated[0][0], &rotated[0][0] + 16, &shape[0][0]);
    }
};


class Grid {
public:
    std::vector<std::vector<int>> cells;
    std::vector<std::vector<Color>> colors;

    Grid() : cells(gridHeight, std::vector<int>(gridWidth, 0)),
        colors(gridHeight, std::vector<Color>(gridWidth, BLANK)) {
    }

    bool CheckCollision(Tetromino& tetromino, int offsetX, int offsetY) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (tetromino.shape[y][x]) {
                    int newX = tetromino.posX + x + offsetX;
                    int newY = tetromino.posY + y + offsetY;
                    if (newX < 0 || newX >= gridWidth || newY >= gridHeight || (newY >= 0 && cells[newY][newX])) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void LockTetromino(Tetromino& tetromino) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (tetromino.shape[y][x]) {
                    cells[tetromino.posY + y][tetromino.posX + x] = 1;
                    colors[tetromino.posY + y][tetromino.posX + x] = tetromino.color;
                }
            }
        }
    }

    void ClearLines(int& score) {
        for (int y = gridHeight - 1; y >= 0; y--) {
            bool fullLine = true;
            for (int x = 0; x < gridWidth; x++) {
                if (!cells[y][x]) {
                    fullLine = false;
                    break;
                }
            }
            if (fullLine) {
                cells.erase(cells.begin() + y);
                cells.insert(cells.begin(), std::vector<int>(gridWidth, 0));
                colors.erase(colors.begin() + y);
                colors.insert(colors.begin(), std::vector<Color>(gridWidth, BLANK));
                score += 100;
                y++;
            }
        }
    }

    void Draw() {
        for (int y = 0; y < gridHeight; y++) {
            for (int x = 0; x < gridWidth; x++) {
                if (cells[y][x]) {
                    DrawRectangle(x * blockSize, y * blockSize, blockSize - 2, blockSize - 2, colors[y][x]);
                }
            }
        }
    }
};


int main() {
    InitWindow(screenWidth, screenHeight, "Tetris - OOP Version");
    SetTargetFPS(60);
    srand(time(nullptr));

    Grid grid;
    Tetromino tetromino;
    int score = 0;
    float timer = 0;
    float delay = 0.5f;

    while (!WindowShouldClose()) {
        
        if (IsKeyPressed(KEY_LEFT) && !grid.CheckCollision(tetromino, -1, 0)) tetromino.posX--;
        if (IsKeyPressed(KEY_RIGHT) && !grid.CheckCollision(tetromino, 1, 0)) tetromino.posX++;
        if (IsKeyPressed(KEY_DOWN) && !grid.CheckCollision(tetromino, 0, 1)) tetromino.posY++;
        if (IsKeyPressed(KEY_UP)) {
            tetromino.Rotate();
            if (grid.CheckCollision(tetromino, 0, 0)) {
                tetromino.Rotate();
                tetromino.Rotate();
                tetromino.Rotate(); 
            }
        }

        
        timer += GetFrameTime();
        if (timer > delay) {
            timer = 0;
            if (!grid.CheckCollision(tetromino, 0, 1)) {
                tetromino.posY++;
            }
            else {
                grid.LockTetromino(tetromino);
                grid.ClearLines(score);
                tetromino.Spawn();
                if (grid.CheckCollision(tetromino, 0, 0)) {
                   
                    grid = Grid();
                    score = 0;
                }
            }
        }

        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        grid.Draw();
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (tetromino.shape[y][x]) {
                    DrawRectangle((tetromino.posX + x) * blockSize, (tetromino.posY + y) * blockSize, blockSize - 2, blockSize - 2, tetromino.color);
                }
            }
        }
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
