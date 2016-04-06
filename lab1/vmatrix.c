#include <mpi.h>
#include <stdio.h>

#define SIZE 8

int MATRIX[SIZE][SIZE];
int VECTOR[SIZE];
int RESULT[SIZE];

void print_matrix(int m[SIZE][SIZE]) { 
	int i, j = 0;
	for (i=0; i<SIZE; i++) {
		printf("\n\t| "); 
		for (j=0; j<SIZE; j++) {
			printf("%2d ", m[i][j]);
		}
		printf("|");
	}
}

void print_vector(int v[SIZE]) {
	int i = 0;
	printf("\n\t| ");
	for (i = 0; i < SIZE; i++) {
		printf("%2d ", v[i]);
	}
	printf("|");
}

void print_vector_long(int v[SIZE]) {
	int i = 0;
	printf("\n\t| ");
	for (i = 0; i < SIZE; i++) {
		printf("%2d ", v[i]);
	}
	printf("|");
}

void fill_matrix(int m[SIZE][SIZE]) {
	int n = 1;
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			//m[i][j] = n++;
			m[i][j] = n;
		}
	}
}

void fill_vector(int v[SIZE]) {
	int n = 0;
	int i;
	for (i = 0; i < SIZE; i++) {
		v[i] = n++;
	}
}

int multiple_row_by_vector(int row[SIZE], int v[SIZE]) {
	int result = 0;
	int i;
	for (i = 0; i < SIZE; i++) {
		result += row[i] * v[i];
	}
	return result;
}


int main(int argc, char *argv[]) {
	int myrank, P, from, to, i, j, k;
	int tag = 666;								/* any value will do */
	int root = 0;
	MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &P);			/* number of processors */

	if (SIZE % P != 0) {
	    if (myrank == root) 
			printf("Matrix size not divisible by number of processors\n");
		MPI_Abort(MPI_COMM_WORLD, -1);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);		/* who am i */

	int row_per_process = SIZE / P;
	from = myrank * row_per_process;
	to = (myrank + 1) * row_per_process;

	if (myrank == root) {
		fill_matrix(MATRIX);
		fill_vector(VECTOR);
	}

	int result[row_per_process];
	int rows[SIZE][row_per_process];
	MPI_Bcast (VECTOR, SIZE, MPI_INT, root, MPI_COMM_WORLD);

	MPI_Scatter (MATRIX, SIZE * row_per_process, MPI_INT, rows, SIZE * row_per_process, MPI_INT, root, MPI_COMM_WORLD);
	   
	printf("computing slice %d (from row %d to %d)\n", myrank, from, to-1);

	for (i = 0; i < row_per_process; i++) {
		result[i] = multiple_row_by_vector(rows[i], VECTOR);
	}
			 
	MPI_Gather (result, row_per_process, MPI_INT, RESULT, row_per_process, MPI_INT, root, MPI_COMM_WORLD);
	   
	if (myrank == 0) {
		printf("\n\n");
		print_matrix(MATRIX);
		printf("\n\n\t       * \n");
		print_vector(VECTOR);
		printf("\n\n\t       = \n");
		print_vector_long(RESULT);
		printf("\n\n");
	} 
	   
	MPI_Finalize();
	return 0;
}
