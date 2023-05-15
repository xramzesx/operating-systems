#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

//// PARAMS ////

#define ELF_COUNT 10
#define REINDEER_COUNT 9

#define WORKSHOP_QUEUE_SIZE 3

//// GLOBAL ////

static pthread_t * threads = NULL;

//// MUTEXES ////

static pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  santa_cond  = PTHREAD_COND_INITIALIZER;

static pthread_cond_t workshop_queue_cond[WORKSHOP_QUEUE_SIZE];

static int workshop_queue[WORKSHOP_QUEUE_SIZE];
static int workshop_index = 0;

static pthread_cond_t reindeers_cond = PTHREAD_COND_INITIALIZER;
static int available_reindeers = 0;

static int remaining_prizes = 3;

//// THREADS ////

static pthread_t * elf_threads = NULL; 
static pthread_t * reindeer_threads = NULL; 
static pthread_t * santa_thread = NULL;

//// ROUTINE ARGUMENT ////

typedef struct {
    int id;
} Args;

//// SETUP SIGNALS ////

void signal_handler(int signo) {
    pthread_mutex_unlock(&main_mutex);
    pthread_exit(NULL);
}

void setup_signal() {
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = signal_handler;
    sigaction(SIGUSR1, &sa, NULL);
}

//// ROUTINES ////

void * santa_routine(void * vargs) {
    Args * args = (Args *) vargs;

    pthread_mutex_lock( &main_mutex );

    while (remaining_prizes) {
        printf("[santa] zasypia\n");

        while( workshop_index < WORKSHOP_QUEUE_SIZE && available_reindeers < REINDEER_COUNT )
            pthread_cond_wait(&santa_cond, &main_mutex);
        printf("[santa] wybudza sie\n");

        if ( workshop_index >= WORKSHOP_QUEUE_SIZE ) {
            printf(
                "[santa] rozwiazuje problemy elfow: %d, %d, %d\n", 
                workshop_queue[0],
                workshop_queue[1],
                workshop_queue[2]
            );
            
            for (int i = 0; i < WORKSHOP_QUEUE_SIZE; i++ ) {

                printf("[santa] elf: %d : mikolaj rozwiazuje jego problem\n", workshop_queue[i]);
                pthread_cond_signal(&workshop_queue_cond[i]);
                pthread_cond_wait(&santa_cond, &main_mutex);
                sleep(rand() % 2 + 1);
                pthread_cond_signal(&workshop_queue_cond[i]);
                workshop_queue[i] = 0;
            }

            workshop_index = 0;
        }

        if (available_reindeers >= REINDEER_COUNT ) {
            printf("[santa] dostarczam zabawki\n");
            
            // Deliver prizes //
            sleep(rand() % 3 + 2);
            available_reindeers = 0;
            remaining_prizes--;
            for (int i = 0; i < REINDEER_COUNT; i++)
                pthread_cond_signal(&reindeers_cond);
        }
    }

    printf("[santa] Mikolaj zamyka biznes, zwalnia elfy, wypuszcza renifery i wyjezdza w Bieszczady\n");
    pthread_mutex_unlock(&main_mutex);

    for (int i = 0; i < ELF_COUNT; i++) {
        pthread_join(elf_threads[i], NULL);
    }

    for (int i = 0; i < REINDEER_COUNT; i++) {
        pthread_join(reindeer_threads[i], NULL);
    }
}

void * reindeer_routine(void * vargs) {
    setup_signal();

    Args * args = (Args *) vargs;

    while( true ) {
        printf("[reindeer] id: %d wyruszyl na wakacje\n", args->id);
        
        // Chilling for 5-10s //
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&main_mutex);

        if (!remaining_prizes) {
            pthread_mutex_unlock(&main_mutex);
            break;
        }

        available_reindeers++;

        printf("[reindeer] id: %d, czeka %d reniferow na Mikolaja\n", args->id, available_reindeers);
        
        if (available_reindeers == REINDEER_COUNT ) {
            printf("[reindeer] id: %d budzi Mikolaja\n", args->id);
            pthread_cond_signal(&santa_cond);
        }
    
        pthread_cond_wait(&reindeers_cond, &main_mutex);
        pthread_mutex_unlock(&main_mutex);
    }

    printf("[reindeer] id: %d zostal do konca na wakacjach\n", args->id);
}

void * elf_routine ( void * vargs ) {
    setup_signal();

    Args * args = (Args *) vargs;

    printf("[elf] id: %d powstal z martwych!\n", args->id);
    fflush(stdout);

    while (true) {
        // Working for 2-5s //
        sleep(rand() % 4 + 2);

        pthread_mutex_lock(&main_mutex);

        if (!remaining_prizes) {
            pthread_mutex_unlock(&main_mutex);
            break;
        }

        if ( workshop_index < WORKSHOP_QUEUE_SIZE) {

            pthread_cond_t * queue_cond = &workshop_queue_cond[workshop_index];
            workshop_queue[workshop_index] = args->id;
            
            workshop_index++;

            if (workshop_index == WORKSHOP_QUEUE_SIZE)
                printf("[elf] id: %d: wybudzam Mikolaja\n", args->id);

            pthread_cond_signal(&santa_cond);
            pthread_cond_wait(queue_cond, &main_mutex);
            
            printf("[elf] id: %d : Mikolaj rozwiazuje problem \n", args->id);

            pthread_cond_signal(&santa_cond);
            pthread_cond_wait(queue_cond, &main_mutex);

        } else {
            /// elf solve problem by himself for 1-2s///
            // sleep(rand() % 2 + 1);
            // printf("[elf] id: %d sam sobie poradzil\n", args->id);
        }

        pthread_mutex_unlock(&main_mutex);
    }
    
    printf("[elf] id: %d spoczywa w spokoju!\n", args->id);
    fflush(stdout);
}

int main (int argc, char ** argv) {
    srand(time(NULL));

    /// INIT QUEUE ///

    for (int i = 0; i < WORKSHOP_QUEUE_SIZE; i++) {
        workshop_queue[i] = 0;
        pthread_cond_init(&workshop_queue_cond[i], NULL);
    }

    //// INIT THREADS ////

    elf_threads = calloc(ELF_COUNT, sizeof(pthread_t));
    reindeer_threads = calloc(REINDEER_COUNT, sizeof(pthread_t));
    santa_thread = calloc(1, sizeof(pthread_t));

    //// SPAWN THREADS ////

    pthread_create(santa_thread, NULL, santa_routine, NULL);

    for (int i = 0; i < ELF_COUNT; i++) {
        Args * args = calloc(1, sizeof(Args));
        args->id = i;
        pthread_create(&elf_threads[i], NULL, elf_routine, args);
    }

    for (int i = 0; i < REINDEER_COUNT; i++) {
        Args * args = calloc(1, sizeof(Args));
        args->id = i;
        pthread_create(&reindeer_threads[i], NULL, reindeer_routine, args);
    }

    //// WAIT FOR THREADS ////

    pthread_join(*santa_thread, NULL);

    //// CLEAN MEMORY ////

    free(elf_threads);
    free(reindeer_threads);
    free(santa_thread);

    pthread_mutex_destroy(&main_mutex);

    return 0;
}