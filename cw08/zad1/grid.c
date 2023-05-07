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

const int grid_width = 30;
const int grid_height = 30;

//// GRID CONTROLS ////

char * source_grid;
char * destination_grid;

bool is_running = true;

//// THREADS ////

const int threads_length = grid_height * grid_width;
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
            point->row * grid_width + point->column
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

    for (int i = 0; i < grid_height; i++ ) {
        for (int j = 0; j < grid_width; j++ ) {            
            Point * args = calloc(1, sizeof(Point));
            args->row = i;
            args->column = j;
            pthread_create( &threads[i * grid_width + j], NULL, routine, args );
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
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid) {
    free(grid);
}

void draw_grid(char *grid) {
    for (int i = 0; i < grid_height; ++i) {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j) {
            if (grid[i * grid_width + j]) {
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
    for (int i = 0; i < grid_width * grid_height; ++i)
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
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width) {
                continue;
            }
            if (grid[grid_width * r + c]) {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col]) {
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