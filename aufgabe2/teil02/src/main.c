/*
 * main.c
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "thpool.h"
#include "my_error.h"
#include "my_mutex.h"
#include "taskqueue.h"
#include "getcharTimeout.h"

#define CONSUMERTASKS   "/consumertask"
#define PRODUCERTASKS   "/producertask"
#define ALPHABET_LENGHT 26
#define SLEEP_CONSUMER  2
#define SLEEP_PRODUCER  3

pthread_t thread_control;
struct threadpool * consumerPool;
struct threadpool * producerPool;
mqd_t producertasks;
mqd_t consumertasks;
int count = 0;

bool finished = false;

void
quit_task () {
    printf("Quit task...\n");

    thpool_destroy(consumerPool); 
    thpool_destroy(producerPool); 
    my_clean();
    finished = true;
    printf("finish\n");
}

void
producer_task (void * argument) {
    printf("Producer Task...\n");
    char * character = (char *)argument;

    if (*character == 'a') {
        my_producer(*character + (count % ALPHABET_LENGHT));
    } else { 
        my_producer(*character + (count % ALPHABET_LENGHT));
    }
    count++;
}

void
consumer_task() {
    printf("Consumer Task started...\n");

    my_consumer();
}

void *
control_thread () {
    char eingabe = '0';
    bool producer = true;
    bool big = true;
    bool consumer = true;


    char argument = 'A';
    struct task job = {NULL, NULL};

    while (true) {
        if (big) {
            argument = 'A';
        } else {
            argument = 'a';
        }

        if (producer) {
            job.routineForTask = producer_task;
            job.arg = &argument;
            thpool_add_task(producerPool, job, 0);
        }

        if (consumer) {
            job.routineForTask = consumer_task;
            job.arg = NULL;
            thpool_add_task(consumerPool, job, 0);
        }

        eingabe = getcharTimeout(1);
        switch (eingabe) {
            case 'p':
            case 'P':
                if (producer && big) {
                    big = false;
                } else if (producer && !big) { 
                    producer = false;
                } else if (!producer && !big){
                    big = true;
                    producer = true;
                }
                break;
            case 'c':
            case 'C':
                if (consumer) {
                    consumer = false;
                } else { 
                    consumer = true;
                }
                break;
            case 'q':
            case 'Q':
                job.routineForTask = quit_task;
                job.arg = NULL;
                thpool_add_task(consumerPool, job, 1);
                pthread_exit(0);
                break;
            case 'h':
            case 'H':
                printf("-----------HELP----------\
                      \n-1   Start/Stop Producer_1\
                      \n-2   Start/Stop Producer_2\
                      \n-c/C Start/Stop Consumer\
                      \n-q/Q Quit Main Thread\n");
                break;
            default:
                break;
        }
    }
}

int
main (void) {
    printf("---------------------------MAIN---------------------------\n");

    destroyTaskQueue(CONSUMERTASKS);
    destroyTaskQueue(PRODUCERTASKS);

    producertasks = createTaskQueue(PRODUCERTASKS, 10);
    consumertasks = createTaskQueue(CONSUMERTASKS, 10);
    printf("Init\n");
    my_initiate();

    producerPool = thpool_create(producertasks);
    consumerPool = thpool_create(consumertasks);
    printf("Create Threads\n");
    pthread_create(&thread_control, NULL, control_thread, NULL);

    pthread_join(thread_control, NULL);

    while(true) {
        if (finished) {
            break;
        }
    }

    destroyTaskQueue(CONSUMERTASKS);
    destroyTaskQueue(PRODUCERTASKS);

    return 0;


}
