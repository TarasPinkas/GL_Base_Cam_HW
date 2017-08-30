#ifndef _COMMON_H
#define _COMMON_H

#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int print(const char str[], ...)
{
	char buf[MAX_MESSAGE_SIZE] = {0};
	int characters = 0;

	va_list args;
	va_start(args, str);

	characters = vsnprintf(buf, MAX_MESSAGE_SIZE, str, args);

	va_end(args);

	if (characters > 0)
	{
		pthread_mutex_lock(&mutex);
			printf("%.*s", characters, buf);
		pthread_mutex_unlock(&mutex);
	}
	return characters;
}

#endif   /* _COMMON_H */
