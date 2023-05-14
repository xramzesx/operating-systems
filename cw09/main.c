#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

//// PARAMS ////

#define ELF_COUNT 20
#define REINDEER_COUNT 20

#define WORKSHOP_QUEUE_SIZE 3

//// GLOBAL ////

static pthread_t * threads = NULL;

//// MUTEXES ////

static pthread_mutex_t workshop_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  workshop_santa_cond  = PTHREAD_COND_INITIALIZER;

static int workshop_queue[WORKSHOP_QUEUE_SIZE];
static int workshop_index = 0;

static pthread_cond_t workshop_queue_cond[WORKSHOP_QUEUE_SIZE];

//// ROUTINE ARGUMENT ////

typedef struct {
    int id;
} Args;

//// ROUTINES ////

void * santa_routine(void * vargs) {
    Args * args = (Args *) vargs;

    int prizes = 3;
    pthread_mutex_lock( &workshop_mutex );

    while (prizes) {
        printf("[santa] zasypia\n");

        while( workshop_index < WORKSHOP_QUEUE_SIZE )
            pthread_cond_wait(&workshop_santa_cond, &workshop_mutex);
        printf("[santa] wybudza sie\n");

        if ( workshop_index >= WORKSHOP_QUEUE_SIZE ) {


            for (int i = 0; i < WORKSHOP_QUEUE_SIZE; i++ ) {

                printf("[santa] elf: %d : mikolaj rozwiazuje jego problem\n", workshop_queue[i]);
                pthread_cond_signal(&workshop_queue_cond[i]);
                pthread_cond_wait(&workshop_santa_cond, &workshop_mutex);
                sleep(rand() % 2 + 1);
                pthread_cond_signal(&workshop_queue_cond[i]);
                workshop_queue[i] = 0;
            }

            workshop_index = 0;
        }
        prizes--;
    }

    pthread_mutex_unlock(&workshop_mutex);

}

void * reindeer_routine(void * vargs) {
    Args * args = (Args *) vargs;

}

void * elf_routine ( void * vargs ) {

    Args * args = (Args *) vargs;

    printf("[elf] id: %d powstał z martwych!\n", args->id);
    fflush(stdout);

    // Working for 2-5s //
    sleep(rand() % 4 + 2);

    pthread_mutex_lock(&workshop_mutex);

    if ( workshop_index < WORKSHOP_QUEUE_SIZE) {
        pthread_cond_t * queue_cond = &workshop_queue_cond[workshop_index];
        workshop_queue[workshop_index] = args->id;
        
        workshop_index++;

        if (workshop_index == WORKSHOP_QUEUE_SIZE)
            printf("[elf] id: %d: wybudzam Mikolaja\n");

        pthread_cond_signal(&workshop_santa_cond);
        pthread_cond_wait(queue_cond, &workshop_mutex);
        
        printf("[elf] id: %d : Mikolaj rozwiazuje problem \n", args->id);
        pthread_cond_signal(&workshop_santa_cond);
        pthread_cond_wait(queue_cond, &workshop_mutex);

    } else {
        /// elf solve problem by himself for 1-2s///
        // sleep(rand() % 2 + 1);
    }

    pthread_mutex_unlock(&workshop_mutex);

    printf("[elf] id: %d spoczywa w spokoju!\n", args->id);
    fflush(stdout);

}

static pthread_t * elf_threads = NULL; 
static pthread_t * reindeer_threads = NULL; 
static pthread_t * santa_thread = NULL;

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

    for (int i = 0; i < ELF_COUNT; i++) {
        pthread_join(elf_threads[i], NULL);
    }

    for (int i = 0; i < REINDEER_COUNT; i++) {
        pthread_join(reindeer_threads[i], NULL);
    }

    free(elf_threads);
    free(reindeer_threads);
    free(santa_thread);

    return 0;
}