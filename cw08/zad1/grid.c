#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

//// CONSTANTS ////

#define GRID_WIDTH  30
#define GRID_HEIGHT 30

//// GRID CONTROLS ////

char * source_grid;
char * destination_grid;

bool is_running = true;

//// THREADS ////

const int threads_length = GRID_HEIGHT * GRID_WIDTH;
static pthread_t * threads;


void signal_handler(int signo) {}

void * routine( void * args ) {
    //// SETUP SIGNAL ////
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = signal_handler;
    sigaction(SIGUSR1, &sa, NULL);

    //// RECEIVE REQUESTS ////

    Point * point = (Point *) args;

    while( is_running ) {
        pause();

        destination_grid[
            point->row * GRID_WIDTH + point->column
        ] = is_alive(
            point->row, 
            point->column, 
            source_grid
        );
    }

    //// CLEANING ////

    free( point );
    return 0;
}

void ping_threads() {
    for (int i = 0; i < threads_length; i++) {
        pthread_kill(threads[i], SIGUSR1);
    }
}

void create_threads() {
    threads = calloc( threads_length, sizeof(pthread_t));

    for (int i = 0; i < GRID_HEIGHT; i++ ) {
        for (int j = 0; j < GRID_WIDTH; j++ ) {            
            Point * args = calloc(1, sizeof(Point));
            args->row = i;
            args->column = j;
            pthread_create( &threads[i * GRID_WIDTH + j], NULL, routine, args );
        }
    }
}

void stop_threads() {
    is_running = false;

    printf("[threads] joining threads\n");

    ping_threads();

    for (int i = 0; i < threads_length; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[threads] joining finished\n");
}

//// GAME OF LIFE ////

char *create_grid() {
    return malloc(sizeof(char) * GRID_WIDTH * GRID_HEIGHT);
}

void destroy_grid(char *grid) {
    free(grid);
}

void draw_grid(char *grid) {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < GRID_WIDTH; ++j) {
            if (grid[i * GRID_WIDTH + j]) {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            } else {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid) {
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid) {

    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= GRID_HEIGHT || c < 0 || c >= GRID_WIDTH) {
                continue;
            }
            if (grid[GRID_WIDTH * r + c]) {
                count++;
            }
        }
    }

    if (grid[row * GRID_WIDTH + col]) {
        return count == 2 || count == 3; 
    } else {
        return count == 3;
    }
}

void update_grid(char *src, char *dst) {
    source_grid = src;
    destination_grid = dst;

    ping_threads();
}