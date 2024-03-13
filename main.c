#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define GRID_SIZE 10
#define CELL_SIZE 50
#define TEXT_SIZE 15

typedef struct {
  int x;
  int y;
  char txt[10];
  Color color;
  float scale;
} cell;

Vector2 cell2screen(cell p) {
  return (Vector2) {
    .x = (p.x-1) * CELL_SIZE,
    .y = (GRID_SIZE - (p.y-1)) * CELL_SIZE,
  };
}

cell screen2cell(Vector2 q) {
  return (cell) {
    .x = q.x / CELL_SIZE + 1,
    .y = GRID_SIZE - q.y / CELL_SIZE + 1,
    .txt = {0},
    .color = LIGHTGRAY,
  };
};

void draw_cell(cell *pt) {
  cell p = *pt; // copy
  p.y += 1;
  float lw = 1; // line width
  float padding = lw + CELL_SIZE * (1-p.scale);
  Vector2 q = cell2screen(p);
  DrawRectangle(q.x + padding, q.y +padding , CELL_SIZE - 2*padding, CELL_SIZE-2*padding, p.color);
  int textWidth = MeasureText(p.txt, TEXT_SIZE);
  int textX = q.x + (CELL_SIZE - textWidth) / 2;
  int textY = q.y + (CELL_SIZE - TEXT_SIZE) / 2;
  DrawText(p.txt, textX, textY, TEXT_SIZE, BLACK);
}

void draw_grid() {
  for (int x = 1; x <= GRID_SIZE; ++x) {
    for (int y = 1; y <= GRID_SIZE; ++y) {
      cell p = {x,y, {0}, LIGHTGRAY};
      p.y += 1;
      Vector2 q = cell2screen(p);
      DrawRectangleLines(q.x, q.y, CELL_SIZE, CELL_SIZE, BLACK);
    }
  }
}

cell get_mouse_cell() {
  return screen2cell(GetMousePosition());
};

#define MAX_CELLS 100
cell CELLS[MAX_CELLS] = {0};
int CELLS_IDX = 0;

void cells_clear() {
  memset(CELLS, 0, sizeof(CELLS));
  CELLS_IDX = 0;
}

cell *cells_new() {
  cell *p = &CELLS[CELLS_IDX];
  CELLS_IDX++;
  *p = (cell) {
    .x = 0,
    .y = 0,
    .txt = {0},
    .color = BLUE,
    .scale = 1
  };
  return p;
}

int main(void) {
  // Initialization
  const int screenWidth = GRID_SIZE * CELL_SIZE;
  const int screenHeight = GRID_SIZE * CELL_SIZE;
  InitWindow(screenWidth, screenHeight, "Multiply Game");
  SetTargetFPS(60); // Set target frames-per-second
  int phase = 0;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_grid();
    cell mp = get_mouse_cell();
    if (phase == 0) {
      if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        phase = 1;
      }
      cells_clear();
      for (int y = 1; y <= mp.y; ++y) {
        for (int x = 1; x <= mp.x; ++x) {
          cell *p = cells_new();
          p->x = x;
          p->y = y;
          if (x == 1 && y == mp.y) {
            snprintf(p->txt, 8, "%d", y);
          }
          if (x == mp.x && y == 1) {
            snprintf(p->txt, 8, "%d", x);
          }
          if (x == mp.x && y == mp.y) {
            p->color = RED;
            snprintf(p->txt, 8, "%dx%d",x,y);
          }
          if (x <= mp.x && y <= mp.y ) {
            draw_cell(p);
          }
        }
      }
    } else if (phase == 1) {
      if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        phase = 0;
      }
      for(int i = 0; i < CELLS_IDX; i++) {
        cell *p = &CELLS[i];
        p->scale = 0.95;
        sprintf(p->txt, "%d", i+1);
        p->color = BLUE;
        draw_cell(p);
      }
    }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
