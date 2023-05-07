#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

//// GLOBAL ///

char *foreground = NULL;
char *background = NULL;

bool simulation_state = true;

//// HANDLER ////

void handle_sigint(int signo);

//// MAIN ////

int main() {
	
	//// INIT ////

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	//// SIGNAL ////

	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);

	//// THREADS & GRID ////

	create_threads();

	foreground = create_grid();
	background = create_grid();
	
	init_grid(foreground);
	
	//// SIMULATION ////

	while (simulation_state) {
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		update_grid(foreground, background);
		char * tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	
	stop_threads();

	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}

//// HANDLER BODY ////

void handle_sigint(int signo) {
	simulation_state = false;
}