/*
 * my_error.h
 *  Created on: 05.05.2018
 *      Author: Janaina Flor Kaufmann
 */


#ifndef _MY_ERROR_H
#define _MY_ERROR_H
#include <errno.h>

#define RED     "\x1B[31m"
#define RESET   "\x1B[0m"

#define errorhandler(code, message) if((errno = code) != 0)perror(message)

#endif
