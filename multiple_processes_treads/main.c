#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>


#define THREAD_QUANTITY 5

static int thread_status = 1;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sig_handler(int sig)
{
	printf("\rGet SIGINT(%d). Threads stoped...\n", sig);
	thread_status = 0;
}


int get_id(void)
{
	static int id = 0;
	int local_id = 0;

	pthread_mutex_lock(&mutex);
		local_id = id++;
	pthread_mutex_unlock(&mutex);

	return local_id;
}

static void *thread_func(void *arg)
{
	while(thread_status)
	{
		printf("Thread %d id = %d\n", (int)((intptr_t)arg), get_id());
		sleep(1);
	}
	return 0;
}


int main(void)
{
	int i = 0;
	int error = 0;
	void *tStatus;
	pthread_t tread[THREAD_QUANTITY];

	signal(SIGINT, sig_handler);

	for (i = 0; i < THREAD_QUANTITY; i++)
	{
		if((error = pthread_create(&tread[i], NULL, thread_func, (void*)((intptr_t)i))))
		{
			printf("Error, %s\n", strerror(error));
		}
	}

	for(i = 0; i < THREAD_QUANTITY; i++)
	{
		if(pthread_join(tread[i], &tStatus))
		{
			printf("Error. Exit code = %d\n", *(int *)tStatus);
		}
	}

	return 0;
}
