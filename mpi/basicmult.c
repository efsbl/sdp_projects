#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

void master(int, int);
void slave(int, int);

int N = 1024;
double *A, *B, *R;

int main(int argc, char *argv[]){

	int rank;
	int P; //cantidad de procesos

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
	int i, j, k;

	A = (double*)malloc(sizeof(double)*N*N);
	B = (double*)malloc(sizeof(double)*N*N);
	R = (double*)malloc(sizeof(double)*N*N);

	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			A[i*N+j] = 1.0;
			B[i*N+j] = 1.0;
		}
	}

	MPI_Bcast(B, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, N*N/P, MPI_DOUBLE, A, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for(i=0; i<N/P; i++){
		for(j=0; j<N; j++){
			R[i*N+j] = 0.0;
			for(k=0; k<N; k++){
				R[i*N+j] = R[i*N+j] + A[i*N+k] * B[k+j*N];	
			}
			
		}
	}

	MPI_Gather(R, N*N/P, MPI_DOUBLE, R, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//Verifica el resultado
  	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			check=check&&(C[i*N+j]==N);
  		 }
  	}

	if(check)
		printf("Multiplicacion de matrices resultado correcto\n");
	else
		printf("Multiplicacion de matrices resultado erroneo\n");

}

void slave(int rank, int P){
	int i, j, k;
	A = (double*)malloc(sizeof(double)*N*N/P);
	B = (double*)malloc(sizeof(double)*N*N);
	R = (double*)malloc(sizeof(double)*N*N);

	MPI_Bcast(B, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, N*N/P, MPI_DOUBLE, A, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for(i=0; i<N/P; i++){
		for(j=0; j<N; j++){
			R[i*N+j] = 0.0;
			for(k=0; k<N; k++){
				R[i*N+j] = R[i*N+j] + A[i*N+k] * B[k+j*N];	
			}
		}
	}

	MPI_Gather(R, N*N/P, MPI_DOUBLE, R, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

}