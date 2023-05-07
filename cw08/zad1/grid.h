#pragma once
#include <stdbool.h>
#include <stdio.h>

//// POINTS ////

typedef struct {
    size_t row;
    size_t column;
} Point;

//// THREADS ////

void create_threads();
void stop_threads();

//// GAME OF LIFE ////

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);