#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

int wait_for_thread_to_start = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct multiply_params {
  int* line; //line where the results are written too
  int num_lines; //Number of lines
  int which_lines; //Lines which are worked on
  int *matrix_1; //Matrix 1
  int *matrix_2; //matrix_2

};


void print_matrix(int n, int m, int* M) {
	int x;
	int y;
	printf("\n");
	for (y=0; y < n; y++) {
		for (x = 0; x < m; x++) {
			printf("%15d", M[y*m+x]);
		}
		printf("\n");
	}
	printf("\n");
}

int* create_matrix_1(int len) { //Creates one of the two Matrizes necessary
  int* matrix_1 = (int*)malloc(len*len*sizeof(int));
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      *(matrix_1 + i * len + j) = rand() % (INT_MAX / 100000); //Random limits so, we dont get to nig values which multiply to overflow...
    }
  }
  printf("Matrix 1:\n");
  print_matrix(len, len, matrix_1);
  return matrix_1;
}

int* create_matrix_2(int len) { //Creates the other one
  int* matrix_2 = (int*)malloc(len*len*sizeof(int));
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      *(matrix_2 + i * len + j) = rand() % (INT_MAX / 100000);
    }
  }
  printf("Matrix 2:\n");
  print_matrix(len, len, matrix_2);
  return matrix_2;
}

void multiply_line(void* thread_args) {
  struct multiply_params* mp = (struct multiply_params*) thread_args;
  int* line = mp->line;
  int len = mp->num_lines;
  int aufruf = mp->which_lines;
  int *matrix_1 = mp->matrix_1;
  int *matrix_2 = mp->matrix_2;
  //Now that we have all values, the loop to assign values, can continue
  pthread_mutex_lock(&mutex);
  wait_for_thread_to_start = 1;
  pthread_mutex_unlock(&mutex);
  for(int i = 0; i < len; i++) {
    int dotproduct_for_one = 0; //Value of one field
    for(int j = 0; j < len; j++) {
      int value_matrix_1 = *(matrix_1 + aufruf*len + j);
      int value_matrix_2 = *(matrix_2 + j * len + i);
      //Now multiply the numbery;
      int temp_value = value_matrix_1 * value_matrix_2;
      //printf("Thread %d; Calculating: %d * %d = %d\n",aufruf, value_matrix_1, value_matrix_2, temp_value);
      dotproduct_for_one += temp_value;
    }
    //printf("Putting Result in: %d\n", dotproduct_for_one);
    (*line) = dotproduct_for_one; //Write it to the right position
    *line++;
  }
};

int create_threading(int len) {
  int *matrix_1 = create_matrix_1(len);
  int *matrix_2 = create_matrix_2(len);
  //Create result matrix
  int matrix_result[len][len];
  pthread_t workThreadID;
  struct timespec tstart={0,0}, tend={0,0};
  clock_gettime(CLOCK_MONOTONIC, &tstart);
  for(int i = 0; i < len; i++) {
    int local_wait_for_thread_to_start = 0; //We use a wait lock, to make sure, we don't start another loop, before we have succesfully commuciated all values, to local variables of the thread
    wait_for_thread_to_start = 0;
    struct multiply_params args;
    args.line = matrix_result[i];
    args.num_lines = len;
    args.which_lines = i;
    args.matrix_1 = matrix_1;
    args.matrix_2 = matrix_2;
    //printf("%d\n", args.which_lines);
    pthread_create(&workThreadID, NULL, multiply_line, &args);
    while(!local_wait_for_thread_to_start) {
      pthread_mutex_lock(&mutex);
      local_wait_for_thread_to_start = wait_for_thread_to_start;
      pthread_mutex_unlock(&mutex);
    }
  }
  int threadResult;
  pthread_join(workThreadID, (void*) &threadResult);
  clock_gettime(CLOCK_MONOTONIC, &tend);
  print_matrix(len,len, *matrix_result);
  free(matrix_1);
  free(matrix_2);
  printf("The threaded computation for a %d x %d Matrix Multiplication, took: %f Nanoseconds\n", len, len,((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
  return 1;
};

int main (int argc, char *argv[]) {
  if (argc != 2) printf("Usage: <Name> <Size of Matrizes>");
  int len = atoi(argv[1]);
  srand( (unsigned)time( NULL ) );
  create_threading(len);
  return 0;
}

