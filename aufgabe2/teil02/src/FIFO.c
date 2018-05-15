/*
 * FIFO.c
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann
 */

#include <stdio.h>
#include <stdlib.h>
#include "my_error.h"
#include "FIFO.h"

Queue* dummy = NULL;

void
queue_init() {
    if ((dummy = malloc(sizeof(Queue))) != NULL) {
        dummy->buchstabe = '0';
        dummy->previous = NULL;
        return ;
    } else {
        perror(RED"Fail to initiate Queue!"RESET);
	}
}

void
queue_put(char newCharacter) {
    Queue* newQueue = NULL;
    Queue* curr = dummy;

    if (dummy == NULL) {
        perror(RED"Queue ist not initiatet yet!"RESET);
        return;
    }

    if ((newQueue = malloc(sizeof(Queue))) == NULL) {
        perror(RED"Fail to allocate -newQueue"RESET);
		return;
    }

    newQueue->buchstabe = newCharacter;


	if (dummy->previous == NULL) {
        dummy->previous = newQueue;
        newQueue->previous = NULL;
        return;
    } else {
        while(curr->previous != NULL){
            curr = curr->previous;
        }
        curr->previous = newQueue;
        newQueue->previous = NULL;
    }
}

void
queue_get(char *currChar) {
    Queue* curr = dummy;

	if (dummy->previous != NULL) {
        curr = dummy->previous;
        *currChar = curr->buchstabe;
        dummy->previous = curr->previous;
        free(curr);
    } else {
        *currChar = 0;
        perror(RED"Queue ist empty"RESET);
    }
}

void
queue_clean() {
    char d = 0;

    while (dummy->previous != NULL) {
        queue_get(&d);
    }
    free(dummy);   
}



