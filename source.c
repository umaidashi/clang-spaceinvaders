#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conio.h"

#define SCREEN_WITH (32)
#define SCREEN_HEIGHT (24)

#define FPS (10)
#define INTERVAL (1000000 / FPS)

#define INVADER_COLUMN (11)
#define INVADER_ROW (5)

enum {
  TILE_NONE,
  TILE_INVADER,
  TILE_PLAYER,
  TILE_PLAYER_BULLET,
  TILE_INVADER_BULLET,
  TILE_MAX
};

enum { DIRECTION_RIGHT, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_MAX };

typedef struct {
  int x, y;
} VEC2;
typedef struct {
  int x, y;
  int isDead;
} INVADER;
typedef struct {
  int x, y;
} PLAYER;
typedef struct {
  int x, y;
  int isFired;
} PLAYER_BULLET;
typedef struct {
  int x, y;
  int isFired;
} INVADER_BULLET;

VEC2 directions[] = {
    {1, 0},   // DIRECTION_RIGHT
    {0, 1},   // DIRECTION_DOWN
    {-1, 0},  // DIRECTION_LEFT
};

int screen[SCREEN_HEIGHT][SCREEN_WITH];
INVADER invaders[INVADER_ROW][INVADER_COLUMN];
PLAYER player;
PLAYER_BULLET playerBullet;
INVADER_BULLET invaderBullet[INVADER_COLUMN];

int invadersDirection;
int invadersCount;

const char* tileAA[TILE_MAX] = {
    "　",  // TILE_NONE
    "⭐",  // TILE_INVADER
    "🔺",  // TILE_PLAYERA
    "仝",  // TILE_PLAYER_BULLET
    "＊",  // TILE_INVADER_BULLET
};

void DrawScreen() {
  memset(screen, 0, sizeof screen);
  for (int y = 0; y < INVADER_ROW; y++) {
    for (int x = 0; x < INVADER_COLUMN; x++) {
      if (!invaders[y][x].isDead)
        screen[invaders[y][x].y][invaders[y][x].x] = TILE_INVADER;
    }
  }

  screen[player.y][player.x] = TILE_PLAYER;
  if (playerBullet.isFired) {
    screen[playerBullet.y][playerBullet.x] = TILE_PLAYER_BULLET;
  }

  for (int x = 0; x < INVADER_COLUMN; x++) {
    if (invaderBullet[x].isFired) {
      screen[invaderBullet[x].y][invaderBullet[x].x] = TILE_INVADER_BULLET;
    }
  }

  system("clear");
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WITH; x++) {
      printf("%s", tileAA[screen[y][x]]);
    }
    printf("\n");
  }
}

void init() {
  for (int y = 0; y < INVADER_ROW; y++) {
    for (int x = 0; x < INVADER_COLUMN; x++) {
      invaders[y][x].x = x * 2;
      invaders[y][x].y = y * 2;
      invaders[y][x].isDead = 0;
    };
  }

  invadersDirection = DIRECTION_RIGHT;
  invadersCount = 0;

  player.x = SCREEN_WITH / 2;
  player.y = SCREEN_HEIGHT - 2;

  playerBullet.isFired = 0;

  for (int x = 0; x < INVADER_COLUMN; x++) {
    invaderBullet[x].isFired = 0;
  }
  DrawScreen();
}

int playerBulletIntersectInvaders() {
  for (int y = 0; y < INVADER_ROW; y++) {
    for (int x = 0; x < INVADER_COLUMN; x++) {
      if ((invaders[y][x].x == playerBullet.x) &&
          (invaders[y][x].y == playerBullet.y)) {
        invaders[y][x].isDead = 1;
        playerBullet.isFired = 0; // コメントアウトすると貫通弾に
        return 1;
      }
    }
  }
  return 0;
}

int invaderBulletIntersectPlayer() {
  for (int x = 0; x < INVADER_COLUMN; x++) {
    if (invaderBullet[x].isFired && (invaderBullet[x].x == player.x) &&
        (invaderBullet[x].y == player.y)) {
      return 1;
    }
  }
  return 0;
}

int main() {
  init();

  clock_t lastClock = clock();
  while (1) {
    clock_t nowClock = clock();
    if (nowClock >= lastClock + INTERVAL) {
      lastClock = nowClock;

      if (playerBullet.isFired) {
        playerBullet.y--;
        if (playerBullet.y < 0) {
          playerBullet.isFired = 0;
        }

        playerBulletIntersectInvaders();
      }

      invadersCount++;
      if (invadersCount > 10) {
        invadersCount = 0;
        int nextDirection = invadersDirection;
        for (int y = 0; y < INVADER_ROW; y++) {
          for (int x = 0; x < INVADER_COLUMN; x++) {
            if (invaders[y][x].isDead) continue;
            invaders[y][x].x += directions[invadersDirection].x;
            invaders[y][x].y += directions[invadersDirection].y;

            switch (invadersDirection) {
              case DIRECTION_RIGHT:
                if (invaders[y][x].x >= SCREEN_WITH - 1) {
                  nextDirection = DIRECTION_DOWN;
                }
                break;
              case DIRECTION_DOWN:
                if (invaders[y][x].x >= SCREEN_WITH - 1) {
                  nextDirection = DIRECTION_LEFT;
                }
                if (invaders[y][x].x <= 0) {
                  nextDirection = DIRECTION_RIGHT;
                }
                break;
              case DIRECTION_LEFT:
                if (invaders[y][x].x <= 0) {
                  nextDirection = DIRECTION_DOWN;
                }
                break;
            };
          }
        }
        invadersDirection = nextDirection;

        for (int x = 0; x < INVADER_COLUMN; x++) {
          if (!invaderBullet[x].isFired) {
            invaderBullet[x].isFired = 1;

            if (rand() % INVADER_COLUMN) continue;

            int invaderRow = -1;
            for (int y = 0; y < INVADER_ROW; y++) {
              if (!invaders[y][x].isDead) {
                invaderRow = y;
              }
            }
            if (invaderRow > 0) {
              invaderBullet[x].isFired = 1;
              invaderBullet[x].x = invaders[invaderRow][x].x;
              invaderBullet[x].y = invaders[invaderRow][x].y + 1;
            }
          }
        }
      }

      for (int x = 0; x < INVADER_COLUMN; x++) {
        if (invaderBullet[x].isFired) {
          invaderBullet[x].y++;
          if (invaderBullet[x].y > SCREEN_HEIGHT) {
            invaderBullet[x].isFired = 0;
          }
        }
      }
      if (invaderBulletIntersectPlayer()) {
        init();
        lastClock = clock();
        continue;
      }
      DrawScreen();
    }

    if (kbhit()) {
      switch (getch()) {
        case 'a':
          player.x--;
          break;
        case 'd':
          player.x++;
          break;
        default:
          if (playerBullet.isFired) break;
          playerBullet.isFired = 1;
          playerBullet.x = player.x;
          playerBullet.y = player.y - 1;
          break;
      }
      if (player.x < 0) player.x = 0;
      if (player.x >= SCREEN_WITH) player.x = SCREEN_WITH - 1;

      if (invaderBulletIntersectPlayer()) {
        init();
        continue;
      }
      DrawScreen();
    }
  }
}