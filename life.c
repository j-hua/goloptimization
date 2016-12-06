/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"
#include "util.h"

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/

#include <omp.h>

#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

#define NUM_THREADS 8

char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
	const int LDA = nrows;
    	int curgen;

	
	for (curgen = 0; curgen < gens_max; curgen++){
		int thread_count = NUM_THREADS;
#	pragma omp parallel num_threads(thread_count)	
	thread_routine(outboard, inboard, nrows, ncols, LDA);
 	
		SWAP_BOARDS( outboard, inboard );
	}
	

  	return inboard;
}

void thread_routine(char* outboard, char* inboard, const int nrows, const int ncols, const int LDA){
	int my_rank = omp_get_thread_num();//thread_index 
	int thread_count = omp_get_num_threads();//number of threads
	int stride = nrows/thread_count;
	int start_row = stride*my_rank;
	
	int i, j;

	for (i = start_row; i < start_row + stride; i++) {
		const int inorth = mod (i-1, nrows);
		const int isouth = mod (i+1, nrows);
		for (j = 0; j < ncols; j++) {
			
			const int jwest = mod (j-1, ncols);
			const int jeast = mod (j+1, ncols);
	
			const char neighbor_count = 
			    BOARD (inboard, inorth, jwest) + 
			    BOARD (inboard, inorth, j) + 
			    BOARD (inboard, inorth, jeast) + 
			    BOARD (inboard, i, jwest) +
			    BOARD (inboard, i, jeast) + 
			    BOARD (inboard, isouth, jwest) +
			    BOARD (inboard, isouth, j) + 
			    BOARD (inboard, isouth, jeast);

			BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));
		}
	}
}



