/**
 * Parallelized Poisson Problems
 * CS 241 - Spring 2016
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef _BMP_H
#define _BMP_H
#include "bmp.h"
#endif
#ifndef _BARRIER_H
#define _BARRIER_H
#include "barrier.h"
#endif
#ifndef _POISSON_H
#define _POISSON_H
#include "poisson.h"
#endif

barrier_t * barr = NULL;
double solve_poisson_helper(poisson_struct *info, int startx, int starty,
                            int width, int height) {
  double **previous = info->previous;
  double **target_image = info->target_image;
  double **current = info->current;

  int x, y;
  double error = 0;

  for (y = starty; y < height + starty; y++) {
    for (x = startx; x < width + startx; x++) {
      current[y][x] =
          0.25 * (previous[y][x + 1] + previous[y][x - 1] + previous[y + 1][x] +
                  previous[y - 1][x] - target_image[y][x]);
      error += fabs(current[y][x] - previous[y][x]) *
               fabs(current[y][x] - previous[y][x]);
    }
  }
  return error;
}

// This is the function we suggest having each
// indiviudal thread run. Inside of it we call
// the helper function, solve_poisson_helper,
// with the apropriate parameters.
void *solve_poisson(void *info_v) {

  poisson_struct *info = (poisson_struct *)info_v;
  int GRANULARITY = info->granularity;
  double EPSILON = info->epsilon;
  int N = info->n;
  int NUM_THREADS = info->num_threads;
  info->iter = 0;
  double error;


  do {
    error = solve_poisson_helper(info, NUM_THREADS, NUM_THREADS, N, N);
    
    barrier_wait(inbarr);
    info->iter++;

    if (info->iter % GRANULARITY == 0)
      write_to_file(info->current, 0, 0, N + 2, N + 2, info->iter);

    double **temp = info->previous;
    info->previous = info->current;
    info->current = temp;
  } while (error > EPSILON);

  double **temp = info->previous;
  info->previous = info->current;
  info->current = temp;

  write_to_file(info->current, 0, 0, N + 2, N + 2, info->iter);
  return info;
}

// Sets up the poisson_struct so it has a 2-d array of pointers for the old
// iteration and the new iteration. Returns arrays of size n+2
// to handle the edge conditions
poisson_struct **poisson_setup(int n, double **target_image, int num_threads,
                               int granularity, double epsilon) {
  int n_temp = n;                                                                                          
  n += 2;
  int i;
  poisson_struct **ps_array = malloc(sizeof(poisson_struct *) * num_threads);
  double *previous = calloc(sizeof(double), n * n);
  double *current = calloc(sizeof(double), n * n);

  double **previous_ret = malloc(sizeof(double *) * n);
  double **current_ret = malloc(sizeof(double *) * n);
  int actual = 0;
  // This is the abstraction of a 1-D array of memory into a 2-D array
  for (i = 0; i < n; i++, actual += n) {
    previous_ret[i] = &previous[actual];
    current_ret[i] = &current[actual];
  }

  for (i = 0; i < num_threads; i++) {
    ps_array[i] = malloc(sizeof(poisson_struct));
    ps_array[i]->n = n_temp;
    ps_array[i]->num_threads = num_threads;
    ps_array[i]->granularity = granularity;
    ps_array[i]->epsilon = epsilon;
    ps_array[i]->previous = previous_ret;
    ps_array[i]->current = current_ret;
    ps_array[i]->target_image = target_image;
    ps_array[i]->iter = 0;
  }
  return ps_array;
}

// Cleans up the abstraction of a 1-d array
// into a 2-d array that we created in
// poisson_setup
void poisson_destroy(poisson_struct **ps_array, int num_threads) {
  poisson_struct *ps = *ps_array;
  free(*(ps->current));
  free(*(ps->previous));
  free(ps->current);
  free(ps->previous);
  int i;
  for (i = 0; i < num_threads; i++)
  free(ps_array[i]);
  free(ps_array);
}


int serial_poisson(int n, int granularity, double epsilon,
                   double **target_image) {
  poisson_struct **ps_array = poisson_setup(n, target_image, 1, granularity, epsilon, NULL);
  solve_poisson(ps_array[0]);
  int iter_ret = ps_array[0]->iter;
  poisson_destroy(ps_array, 1);
  return iter_ret;
}

int parallel_poisson(int n, int granularity, double epsilon, double **target_image, int num_threads) {
     barrier_init(barr, num_threads);
	poisson_struct **ps_array = poisson_setup(n, target_image, num_threads,granularity, epsilon);
	pthread_t pth[num_threads];
	for( int i =0; i< num_threads ; i++){
		pthread_create(& pth[i], NULL, solve_poisson, ps_array[i]);
	}
     for(int i = 0; i< num_threads; i++)
	pthread_join(pth[i], NULL);
     return 0;
}





