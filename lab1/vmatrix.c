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

void print_vector_long(long v[SIZE]) {
	int i = 0;
	printf("\n\t| ");
	for (i = 0; i < SIZE; i++) {
		printf("%2ld ", v[i]);
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

	/*
	 * int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
	 * buffer		Starting address of buffer (choice).
	 * count		Number of entries in buffer (integer).
	 * datatype		Data type of buffer (handle).
	 * root			Rank of broadcast root (integer).
	 * comm			Communicator (handle).
	 */

	MPI_Bcast (&VECTOR, SIZE, MPI_INT, root, MPI_COMM_WORLD);

	/*
	 * int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
	 * sendbuf		Address of send buffer (choice, significant only at root).
	 * sendcount	Number of elements sent to each process (integer, significant only at root).
	 * sendtype		Datatype of send buffer elements (handle, significant only at root).
	 * recvcount	Number of elements in receive buffer (integer).
	 * recvtype		Datatype of receive buffer elements (handle).
	 * root			Rank of sending process (integer). 
	 * comm			Communicator (handle). 
	 */

	MPI_Scatter (MATRIX, SIZE * row_per_process, MPI_INT, MATRIX[from], SIZE * row_per_process, MPI_INT, root, MPI_COMM_WORLD);
	   
	printf("computing slice %d (from row %d to %d)\n", myrank, from, to-1);

	for (i = from; i < to; i++) {
		RESULT[i] = 0;
		for (k = 0; k < SIZE; k++) {
			RESULT[i] += MATRIX[i][k] * VECTOR[k];
		}
	}
			 
	/*
	 * int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
	 * sendbuf		Starting address of send buffer (choice).
	 * sendcount	Number of elements in send buffer (integer).
	 * sendtype		Datatype of send buffer elements (handle).
	 * recvcount	Number of elements for any single receive (integer, significant only at root).
	 * recvtype		Datatype of recvbuffer elements (handle, significant only at root).
	 * root			Rank of receiving process (integer). 
	 * comm			Communicator (handle). 
	 */
	long result[row_per_process];
	MPI_Gather (&result, row_per_process, MPI_LONG, RESULT, row_per_process, MPI_LONG, root, MPI_COMM_WORLD);
	   
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
