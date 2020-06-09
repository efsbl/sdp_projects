#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

void master(int, int);
void slave(int, int);
double dwalltime(void);

int N;
double *A;
double totalsum;

int main(int argc, char *argv[]){

	int rank;
	int P; //cantidad de procesos

	if (argc < 2){
   		printf("\n Faltan argumentos:: N dimension del arreglo");
   		return 0;
  	}

  	N = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	if (rank==0)
		master(rank, P);
	else
		slave(rank, P);

	MPI_Finalize();
	return 0;
}

void master(int rank, int P){
	int i;
	int aux = 2;
	double localsum = 0;
	totalsum = 0;
	double avg;

	A = (double*)malloc(sizeof(double)*N);

	//Init array
	for(i=0; i<N; i++){
		A[i] = aux;
		if(aux == 2)
			aux = 4;
		else
			aux = 2;
	}

	MPI_Scatter(A, N/P, MPI_DOUBLE, A, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	for(i = 0; i<N/P; i++){
		localsum += A[i];
	}

	MPI_Reduce(&localsum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	avg = totalsum/N;
	printf("El avg para un vector con valores '2' y '4' alternados es de %.1f", avg);


}

void slave(int rank, int P){
	int i;
	double localsum = 0.0;
	A = (double*)malloc(sizeof(double)*N/P);
	MPI_Scatter(A, N/P, MPI_DOUBLE, A, N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	for(i = 0; i<N/P; i++){
		localsum += A[i];
	}
	MPI_Reduce(&localsum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

}
