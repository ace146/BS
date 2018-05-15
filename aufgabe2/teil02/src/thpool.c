#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "taskqueue.h"
#include "my_error.h"
#include "thpool.h"

struct threadpool *
thpool_create(mqd_t taskqueue) {
    int i;
    struct threadpool * pool;

    if ((pool = malloc(sizeof(struct threadpool))) == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Fail to allocate -pool-");
        goto err; 
    }

    pool->started = 0;
    pool->countTask = 0;

    if ((pool->threads = malloc(sizeof(pthread_t) * NUM_THREADS)) == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Fail to allocate -threads-");
        goto err;
    }

    pool->taskqueue = taskqueue;

    errorhandler(pthread_mutex_init(&(pool->mutex), NULL), 
            "Fail to initialize -mutex-");
    if (errno) {
        goto err;
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&(pool->threads[i]), NULL,
                           thpool_routine, (void *) pool) != 0) {
            thpool_destroy(pool);
            return NULL;
        }
        printf("Thread[%d]\n", i);
        pool->started++; 
    }

    pool->destroy = false;

    return pool;

    err:
        if (pool) {
            thpool_free(pool);
        }
        return NULL;
}

int
thpool_add_task (struct threadpool * pool, struct task job, int prio) {
    if (pool == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Invalid Pool");
        return errno;
    }
    errorhandler(pthread_mutex_lock(&(pool->mutex)),
            "Fail to lock -mutex-");
    sendToTaskQueue(pool->taskqueue, job, prio, false);
    printf("added_task!!!\n");
    pool->countTask++;

    return 0;
}

void *
thpool_routine(void * threadpool) {
    struct threadpool *pool = (struct threadpool *) threadpool;
    struct task job;

    while(true) {


        job = receiveFromTaskQueue(pool->taskqueue);
        pool->countTask--;

        pthread_mutex_unlock(&(pool->mutex));

        (*(job.routineForTask))(job.arg);

        
        if (pool->destroy) {
            break;
        }

    }

    pool->started--;
    pthread_mutex_unlock(&(pool->mutex));
    pthread_exit(0);
    return NULL;
}

int
thpool_destroy(struct threadpool * pool) {
    int i;

    if (pool == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Invalid Pool");
        return errno;
    }

    errorhandler(pthread_mutex_lock(&(pool->mutex)),
            "Fail to lock -mutex-");
    if (errno) {
        return errno; 
    }

    do {
        if (pool->destroy) {
            errno = EACCES;
            perror("Destroy exist already!!");
            break;
        }

        pool->destroy = true;


        errorhandler(pthread_mutex_unlock(&(pool->mutex)), 
                "Fail to unlock  -mutex-");
        if (errno) {
            break;
        }

        for (i = 0; i < NUM_THREADS; i++) {
            errorhandler(pthread_join(pool->threads[i], NULL), 
                "Fail to join");
            printf("Cancel Thread [%d]\n", i);
        }


    } while(0);

    if (!errno) {
        thpool_free(pool);
    }
    return errno;
}

int
thpool_free (struct threadpool * pool) {
    if (pool == NULL) {
        errno = EADDRNOTAVAIL;
        perror("Invalid Pool");
        return errno;
    }

    if (pool->threads) {
        free(pool->threads);
        closeTaskQueue(pool->taskqueue);

        errorhandler(pthread_mutex_unlock(&(pool->mutex)),
                "Fail to lock -mutex-");
    }

    free(pool);
    return 0;
}

