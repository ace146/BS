/*
 * my_mutex.h
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann
 */


#ifndef _MY_MUTEX_H
#define _MY_MUTEX_H

#define RED     "\x1B[31m"
#define GRN     "\x1B[32m"
#define YEL     "\x1B[33m"
#define BLU     "\x1B[34m"
#define MAG     "\x1B[35m"
#define CYN     "\x1B[36m"
#define RESET   "\x1B[0m"


extern void my_initiate (void);
extern void my_producer (char character);
extern void my_consumer ();
extern void my_clean (void);

#endif /* _MY_MUTEX_H */
