/**
 * Parallelized Poisson Problems
 * CS 241 - Spring 2016
 */
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

/**
 * You may modify this struct as you wish. We have provided some fields you may
 * find useful.
 */
typedef struct barrier_t {
  pthread_mutex_t mtx;
  pthread_cond_t cv;
  // Total number of threads
  unsigned n_threads;
  // Increasing or decreasing count
  unsigned count;
  // Keeps track of what usage number of the barrier we are at
  unsigned int times_used;
 // double error;

} barrier_t;

/**
 * You may not modify the function signatures of the 3 functions in this header
 * file and you must implement them in your barrier.c.
 */
void  barrier_destroy(barrier_t *barrier);
void  barrier_init(barrier_t *barrier, unsigned num_threads);
void barrier_wait(barrier_t *barrier);
