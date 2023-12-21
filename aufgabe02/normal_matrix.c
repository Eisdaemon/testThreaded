
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <limits.h>

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

int* create_matrix_1(int len) {
  int* matrix_1 = (int*)malloc(len*len*sizeof(int));
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      *(matrix_1 + i * len + j) = rand()% INT_MAX / 100000;
    }
  }
  printf("Matrix 1:\n");
  print_matrix(len, len, matrix_1);
  return matrix_1;
}

int* create_matrix_2(int len) {
  int* matrix_2 = (int*)malloc(len*len*sizeof(int));
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < len; j++) {
      *(matrix_2 + i * len + j) = rand()% INT_MAX / 100000;
    }
  }
  printf("Matrix 2:\n");
  print_matrix(len, len, matrix_2);
  return matrix_2;
}

int multiblpy_line(int* line, int len, int aufruf, int* matrix_1, int* matrix_2) {
  //printf("%d", *(line+1));
  for(int i = 0; i < len; i++) {
    int dotproduct_for_one = 0;
    for(int j = 0; j < len; j++) {
      int value_matrix_1 = *(matrix_1 + aufruf*len + j);
      int value_matrix_2 = *(matrix_2 + j * len + i);
      //Now multiply the numbery;
      int temp_value = value_matrix_1 * value_matrix_2;
      //printf("Calculating: %d * %d = %d\n", value_matrix_1, value_matrix_2, temp_value);
      dotproduct_for_one += temp_value;
    }
    (*line) = dotproduct_for_one;
    *line++;
  }
  return 1;
};

int create_threading(int len) {
  int *matrix_1 = create_matrix_1(len);
  int *matrix_2 = create_matrix_2(len);
  //Create result matrix
  int lines = len;
  int matrix_result[lines][lines];
  struct timespec tstart={0,0}, tend={0,0};
  clock_gettime(CLOCK_MONOTONIC, &tstart);
  for(int i = 0; i < len; i++) {
    multiblpy_line(matrix_result[i], lines, i, matrix_1, matrix_2);
  }
  clock_gettime(CLOCK_MONOTONIC, &tend);
  print_matrix(len,len, *matrix_result);
  free(matrix_1);
  free(matrix_2);
  printf("The non threaded computation for a %d x %d Matrix Multiplication, took: %f Nanoseconds\n", len, len,((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
  return 1;
};

int main (int argc, char *argv[]) {
  if (argc != 2) printf("Usage: <Name> <Size of Matrizes>");
  int len = atoi(argv[1]);
  srand( (unsigned)time( NULL ) );
  create_threading(len);
  return 0;
}
