#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

enum OperatingModes {
    Range = 1,
    Time,
    Requests,
    Interval,
    Exit
};

typedef enum OperatingModes OperatingModes;

void print_current_time(){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );
    fflush(stdout);
}

int requests_count = 0;
int show_time = 0;

void handler_sigusr1(
    int sig,
    siginfo_t *info,
    void *ucontext
) {
    requests_count++;
    show_time = 0;

    printf("\n==:[Received]:=======\n");
    printf(":[signal]:\t%d\n", info->si_signo);
    printf(":[pid]:\t%d\n", info->si_pid);
    printf(":[sigval]:\t%d\n", info->si_value.sival_int);
    
    fflush(stdout);

    printf("Output:\n");

    switch (info->si_value.sival_int) {
        case Range:
            for (int i = 1; i <= 100; i++ ) {
                printf("%d, ", i);
            }
            printf("\n");
            fflush(stdout);
            break;
        case Time:
            print_current_time();
            break;
        case Requests:
            printf("Requests count: %d\n", requests_count);
            fflush(stdout);
            break;
        case Interval:
            show_time = 1;
            printf("Show current time\n");
            break;
        case Exit:
            printf("process exit\n");
            fflush(stdout);
            kill(info->si_pid, SIGUSR1);
            exit(0);
            break;

        default:
            perror("invalid operating mode\n");
            fflush(stdout);
            break;
    }
    
    // usleep(10000);

    kill(info->si_pid, SIGUSR1);
}


int main () {

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handler_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    printf("[catcher] pid: %d\n", getpid());

    while (1) {
        if ( show_time ) {
            print_current_time();
        }
        sleep(1);
    }

    return 0;
}