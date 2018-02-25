#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "timer.h"

pthread_mutex_t mutex_lock;
int mutex = 0;
int lock = 0;
int unlock = 1;
static volatile int count = 0;


void *cs_func(void *arg)
{
	int i = 0;
	for(i = 0; i < 20000000; i++)
	{
        while (!(__sync_bool_compare_and_swap (&mutex,lock, 1) )) usleep(1000);
         count++;
         __sync_bool_compare_and_swap (&mutex, unlock, 0);
	}
	return NULL;
}

void *lock_func(void *arg)
{
	int i = 0;
	for(i = 0; i < 20000000; i++)
	{
		pthread_mutex_lock(&mutex_lock);
		count++;
		pthread_mutex_unlock(&mutex_lock);
	}
	return NULL;
}

void *ts_func(void *arg)
{
	int i = 0;
	for(i = 0; i < 20000000; i++)
	{
        while (__sync_lock_test_and_set (&mutex,1) ) usleep(1000);
         count++;
        __sync_lock_release(&mutex);
	}
	return NULL;
}

int main(int argc, const char *argv[])
{
	Timer timer;
	timer.Start();
	pthread_t thread_ids[10];
	int i = 0;
	
	if (argc < 2)
	{
		printf("input paramter error! \n");
		printf("eg: ./common cs[lock][ts]\n");
		return 0 ;
	}

	if (!strcmp(argv[1], "cs"))
	{
		printf("compare_and_swap：\n");
		for(i = 0; i < sizeof(thread_ids)/sizeof(pthread_t); i++)
		{
				pthread_create(&thread_ids[i], NULL, cs_func, NULL);
		}
	}
	else if (!strcmp(argv[1], "lock"))
	{
		printf("mutex_lock：\n");
		for(i = 0; i < sizeof(thread_ids)/sizeof(pthread_t); i++)
		{
				pthread_create(&thread_ids[i], NULL, lock_func, NULL);
		}
	}
	else if (!strcmp(argv[1], "ts"))
	{
		printf("test_and_set：\n");
		for(i = 0; i < sizeof(thread_ids)/sizeof(pthread_t); i++)
		{
				pthread_create(&thread_ids[i], NULL, ts_func, NULL);
		}
	}
	else
	{
		printf("input error! \n");
		return 0 ;
	}

	for(i = 0; i < sizeof(thread_ids)/sizeof(pthread_t); i++)
	{
			pthread_join(thread_ids[i], NULL);
	}

	timer.Stop();
	timer.Cost_time();
	printf("结果:count = %d\n",count);

	return 0;
}
 
