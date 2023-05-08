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

int threads_length = grid_height * grid_width;
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

    Range * range = (Range *) args;

    while( is_running ) {
        pause();

        for (int iter = range->start; iter <= range->end; iter ++) {
            int i = iter / grid_width;
            int j = iter % grid_width;

            destination_grid[iter] = is_alive(i, j, source_grid);
        }
    }

    //// CLEANING ////

    free( range );
    return 0;
}

void ping_threads() {
    for (int i = 0; i < threads_length; i++) {
        pthread_kill(threads[i], SIGUSR1);
    }
}

void create_threads(int num_threads) {
    threads_length = num_threads;

    threads = calloc( threads_length, sizeof(pthread_t));

    int area = grid_height * grid_width;
    int difference = area / threads_length;

    Range ** ranges = calloc( threads_length, sizeof(Range *) );
    printf("%d \r\n", area);

    for (int i = 0; i < threads_length; i++) {
        ranges[i] = calloc(1, sizeof(Range));
    }

    for (int i = 1; i < threads_length; i++) {
        ranges[i]->start = ranges[i-1]->start + difference;
    }

    for (int i = threads_length - 2; i >= 0; i--) {
        ranges[i]->end = ranges[i + 1]->start - 1;
    }

    ranges[threads_length - 1]->end = area - 1;

    for (int i = 0; i < threads_length; i++) {        
        printf("%d | %ld | %ld\r\n", i, ranges[i]->start, ranges[i]->end);
        pthread_create(&threads[i], NULL, routine, ranges[i]);
    }

    free(ranges);
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