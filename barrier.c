/**
 * Parallelized Poisson Problems
 * CS 241 - Spring 2016
 */
#include "barrier.h"

void barrier_destroy(barrier_t *t) {
	pthread_mutex_destroy( &t->mtx);
	pthread_cond_destroy( &t->cv);
}

void barrier_init(barrier_t *t, unsigned num_threads) {
	pthread_mutex_init(& t->mtx, NULL);
	pthread_cond_init(& t->cv, NULL);
	t-> times_used = 0;
	t-> count = 0;
     //t->error =0;
	t-> n_threads = num_threads;
	t-> error = 0;
}

void barrier_wait(barrier_t *t, double error) {
	pthread_mutex_lock(& t->mtx);
     t->count++;
     unsigned temp = t->times_used;

	while( t->count == t-> n_threads){
		pthread_cond_broadcast(& t->cv);
          t->count =0;
          t->times_used ++;
	}
<<<<<<< .mine
	pthread_cond_broadcast( &t->cv);
	t->error += error;
    t->times_used += 1;
	t->count = 0;
=======
     while(temp == t->times_used )
	pthread_cond_wait( &t->cv, & t->mtx);

>>>>>>> .r56375
	pthread_mutex_unlock(& t-> mtx);
     

 }
