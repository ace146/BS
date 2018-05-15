/*
 * my_mutex.c
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann
 */


#include <stdio.h>
#include <pthread.h>
//#include <semaphore.h>
#include "my_error.h"
#include "FIFO.h"
#include "my_mutex.h"

#ifdef _SEMAPHORE_H
sem_t semOccupied;
sem_t semFree;
#else
pthread_cond_t cond;
#endif

pthread_mutex_t mutex;

int counter;

void
my_initiate () {
    queue_init();

    counter = 0;

    #ifdef _SEMAPHORE_H
    errorhandler(sem_init(&semFree, 0, PUFFER_SIZE), 
            "Fail to initiate -semFree-");
    errorhandler(sem_init(&semOccupied, 0, 0), 
            "Fail to initiate -semOccupied-");
    #else
	errorhandler(pthread_cond_init(&cond, NULL),
            "Fail to initiate -produce_cond");
    #endif

	errorhandler(pthread_mutex_init(&mutex, NULL),
            "Fail to initiate -mutex-");
}

void
my_producer(char character) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    

    #ifdef _SEMAPHORE_H
    sem_wait(&semFree);
    pthread_mutex_lock(&mutex);
    #else
    pthread_mutex_lock(&mutex);
    while (counter == PUFFER_SIZE) {
    	errorhandler(pthread_cond_wait(&cond, &mutex),
                "Fail to wait -produce_cond-");
    }
    #endif
    queue_put(character);
    counter++;
    printf(BLU "Character produce: %c\
          \nNumber of occupied space on buffer: %d\n\n"RESET , character, counter);
    #ifdef _SEMAPHORE_H
    pthread_mutex_unlock(&mutex);
    sem_post(&semOccupied);
    #else
	pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    #endif


    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

void
my_consumer() {
    char character; 
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    

    #ifdef _SEMAPHORE_H
    sem_wait(&semOccupied);
    pthread_mutex_lock(&mutex);
    #else
    pthread_mutex_lock(&mutex);
	while (counter == 0) {
		errorhandler(pthread_cond_wait(&cond, &mutex),
                "Fail to wait -consume_cond-");
	}
    #endif
    queue_get(&character);
    counter--;
    printf(GRN"Character consume: %c\
          \nNumber of occupied space on buffer: %d\n\n"RESET, character, counter);
    #ifdef _SEMAPHORE_H
    pthread_mutex_unlock(&mutex);
    sem_post(&semFree);
    #else
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    #endif


    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

void
my_clean(){ 
    queue_clean();

    #ifdef _SEMAPHORE_H
    errorhandler(sem_destroy(&semFree),
            "Fail to destroy -semFree-");
    errorhandler(sem_destroy(&semOccupied),
            "Fail to destroy -semOccupied-");
    #else
	errorhandler(pthread_cond_destroy(&cond),
            "Fail to destroy -consume_cond-");
    #endif

    errorhandler(pthread_mutex_destroy(&mutex),
            "Fail to destroy -mutex-");
}
