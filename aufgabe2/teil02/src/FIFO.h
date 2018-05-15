/*
 * FIFO.h
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann
 */

#ifndef _FIFO_H
#define _FIFO_H

//Maximale Puffergroesse
#define PUFFER_SIZE 10

typedef struct queue{
    char buchstabe;
	struct queue *previous;
} Queue;

//Prototypen
extern void queue_init(void);
extern void queue_put(char newCharacter);
extern void queue_get(char *currChar);
extern void queue_clean(void);

#endif /* _FIFO_H */
