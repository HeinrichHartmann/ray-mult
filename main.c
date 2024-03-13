#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define GRID_SIZE 10
#define CELL_SIZE 50
#define TEXT_SIZE 17
#define MAX_CELLS 1000

typedef struct {
  int x;
  int y;
} xy;

typedef struct {
  xy P;
  char txt[10];
  Color color;
  float scale;
  // for animation
  xy Q;
  float t0;
} cell;


cell CELLS[MAX_CELLS] = {0};
int CELLS_IDX = 0;

void init_cell(cell *c) {
  memset(c, 0, sizeof(cell));
  c->color = BLUE;
  c->scale = 1;
}

cell new_cell() {
  cell c;
  init_cell(&c);
  return c;
}

Vector2 xy2screen(xy P) {
  return (Vector2) {
    .x = (P.x-1) * CELL_SIZE,
    .y = (GRID_SIZE - (P.y-1)) * CELL_SIZE,
  };
}

Vector2 cell2screen(cell c) {
  return xy2screen(c.P);
}

xy screen2xy(Vector2 q) {
  return (xy) {
    .x = q.x / CELL_SIZE + 1,
    .y = GRID_SIZE - q.y / CELL_SIZE + 1,
  };
};

void draw_cell(cell *cp) {
  cell c = *cp; // copy
  c.P.y += 1;
  float lw = 1; // line width
  float padding = lw;
  Vector2 q = cell2screen(c);
  DrawRectangle(q.x + padding, q.y +padding , CELL_SIZE - 2*padding, CELL_SIZE-2*padding, c.color);
  int textWidth = MeasureText(c.txt, TEXT_SIZE);
  int textX = q.x + (CELL_SIZE - textWidth) / 2;
  int textY = q.y + (CELL_SIZE - TEXT_SIZE) / 2;
  DrawText(c.txt, textX, textY, TEXT_SIZE, BLACK);
}

void draw_cell_anim(cell *cp, float t1) {
  cell c = *cp; // copy
  c.P.y += 1;
  c.Q.y += 1;
  Vector2 q1 = xy2screen(c.P);
  Vector2 q2 = xy2screen(c.Q);
  // s runs from 0..1 within duration
  float duration = .3; // seconds
  float t = MAX(0, MIN(1, (t1 - c.t0) / duration));
  if(c.t0 == 0) { t = 0; }
  // q runs from q1 ... q2 within duration
  Vector2 q = (Vector2) {
    .x = (1-t)*q1.x + t*q2.x,
    .y = (1-t)*q1.y + t*q2.y,
  };
  // calculate padding
  float lw = 1;
  float scale;
  if (t==0) {
    scale = 1;
  }
  if (t > 0) {
    float scale_min = 0.75;
    scale = (4*(t-0.5)*(t-0.5) + scale_min) / (1+scale_min);
  }
  if (t == 1) {
    scale = 1;
  }
  float padding = lw + CELL_SIZE * (1 - scale) / 2;
  DrawRectangle(q.x + padding, q.y +padding , CELL_SIZE - 2*padding, CELL_SIZE-2*padding, c.color);
  int textWidth = MeasureText(c.txt, TEXT_SIZE);
  int textX = q.x + (CELL_SIZE - textWidth) / 2;
  int textY = q.y + (CELL_SIZE - TEXT_SIZE) / 2;
  DrawText(cp->txt, textX, textY, TEXT_SIZE, BLACK);
}

void draw_grid() {
  for (int x = 1; x <= GRID_SIZE; ++x) {
    for (int y = 1; y <= GRID_SIZE; ++y) {
      Vector2 q = xy2screen((xy) {x, y+1});
      DrawRectangleLines(q.x, q.y, CELL_SIZE, CELL_SIZE, BLACK);
    }
  }
}

xy get_mouse_xy() {
  xy P = screen2xy(GetMousePosition());
  P.x = MAX(1, MIN(GRID_SIZE, P.x));
  P.y = MAX(1, MIN(GRID_SIZE, P.y));
  return P;
};

void cells_clear() {
  CELLS_IDX = 0;
}

cell *cells_new() {
  cell *cp = &CELLS[CELLS_IDX];
  CELLS_IDX++;
  init_cell(cp);
  return cp;
}

cell *cells_get(int idx) {
  if(idx < 0 || idx >= CELLS_IDX) return NULL;
  return &CELLS[idx];
}

int main(void) {
  const int screenWidth = GRID_SIZE * CELL_SIZE;
  const int screenHeight = GRID_SIZE * CELL_SIZE;
  InitWindow(screenWidth, screenHeight, "Multiply Game");
  SetTargetFPS(60);
  int phase = 0;
  float phase2_timer = 0;
  int phase2_counter = 0;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_grid();
    xy mp = get_mouse_xy();
    if (phase == 0) {
      if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        phase = 1;
        phase2_timer = 0;
        phase2_counter = 0;
      }
      cells_clear();
      for (int y = 1; y <= mp.y; ++y) {
        for (int x = 1; x <= mp.x; ++x) {
          cell *cp = cells_new();
          cp->P = (xy){x,y};
          if (x == mp.x) {
            cp->color = ORANGE;
          }
          if (y == mp.y) {
            cp->color = (Color) {0, 121, 150, 255};
          }
          if (x == mp.x && y == mp.y) {
            cp->color = RED;
            snprintf(cp->txt, 8, "%dx%d",x,y);
          }
          if (x <= mp.x && y <= mp.y ) {
            draw_cell(cp);
          }
        }
      }
    }

    else if (phase == 1) {
      if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        phase = 0;
      }
      phase2_timer += 1.0 / 60.0;
      phase2_counter++;
      int frames_per_cell = 5;
      if ((phase2_counter % frames_per_cell) == 0) {
        int idx = (phase2_counter / frames_per_cell) - 1;
        if (idx < CELLS_IDX) {
          cell *cp = cells_get(idx);
          cp->t0 = phase2_timer;
          cp->Q = (xy){(idx % 10) + 1 ,  (idx / 10) + 1};
          snprintf(cp->txt, 8, "%d",idx+1);
        }
      }
      for(int i = 0; i < CELLS_IDX; i++) {
        cell *cp = cells_get(i);
        draw_cell_anim(cp, phase2_timer);
      }
    }

    EndDrawing();
  }
  CloseWindow();
  return 0;
}
