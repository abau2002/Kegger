// FILE: kegger.cpp
// A Bautista, Transy U
// OS Fall 2023
//
//	This is a simulation of threads (workers) taking from a shared resoruce pool (keg)
//	that must then be replinished by the first thread (Bobo) without causing issues like race conditions or deadlock
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <queue>

using namespace std;

#define PINT_MAX 124
#define WORK_MAX 500

sem_t noRefill,kegNotEmpty,wakeBobo;
int kegPints; 

void *workerFn(void *ptr);
void *boboFn(void *ptr);

int main(int argc,char** argv){
	if(argc!=2){
		cout << "Only 1 argument\n";
		exit(1);
	}
	else if(atoi(argv[1])>500 || atoi(argv[1])<1){
		cout << "Num 1-500\n";
		exit(1);
	}
	
	srand(time(NULL));
	pthread_t workers[WORK_MAX],bobo;
	int workerCount = atoi(argv[1]);

	kegPints = PINT_MAX;
	
	sem_init(&kegNotEmpty, 0, 1);
	sem_init(&noRefill, 0, 1);
	sem_init(&wakeBobo, 0, 0);

	cout << "Bobo has set up a keg and fallen asleep!\n";

	pthread_create(&bobo,NULL,boboFn,NULL);
	for(int i=0;i<workerCount;i++){
		pthread_create(&workers[i],NULL,workerFn,(void*) &i);
	}

	/* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
	
	for(int i=0;i<workerCount;i++){
		pthread_join(workers[i],NULL);
	}
	pthread_join(bobo,NULL);

	return 0;
}



void *workerFn(void *ptr){
	int sleepTime,drinkTime,*id;
	id = (int*) ptr;

	while(true){
		cout << "Worker " << id << " is awake\n";
	
		sem_wait(&noRefill); // passes if no one is waiting for a refill

		cout << "Worker " << id << " wants a pint\n";
		if(!kegPints) sem_post(&wakeBobo);
		sem_wait(&kegNotEmpty); // passes if keg is not empty
		
		kegPints--;

		cout << "Worker " << id << " has filled their glass\n";

		if(kegPints) sem_post(&kegNotEmpty);

		sem_post(&noRefill);

	    drinkTime = rand() * 10000;
	    for(int i=0;i<drinkTime;i++);
	    cout << "Worker " << id << " has drank and passed out\n";
		
		sleepTime = rand() * 10000;
	    for(int i=0;i<sleepTime;i++);
	}
}

void *boboFn(void *ptr){
	while(true){
		if(!kegPints){
			cout << "Keg has run dry!\n";
			sem_wait(&wakeBobo);

			cout << "Bobo has been woken up :(\n";
			kegPints = PINT_MAX;
			cout << "Bobo has set up a keg and went back to sleep!\n";

			sem_post(&kegNotEmpty);

		}	
	}
}