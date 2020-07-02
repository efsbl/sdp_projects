
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

//Resolver la expresion R = avgCD * (AA + maxA * CD)

void master(void);
void slave(void);
void imprimir(double *m);
int check(double *A, int n);
double dwalltime(void);
void Multiply(double *X, double *Y, double *Z);

int main(int argc, char *argv[])
{
	unsigned int N; //Matrices de tamaño NxN
	int rank; //Rank o Id de cada proceso dentro del Communicator
	int P; //Número de procesos

	N = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	if (rank == 0)
		master(rank, P, N);
	else
		slave(rank, P, N);

	MPI_Finalize();
	return 0;
}

void master(int rank, int P, int N)
{
	int i, j, k;
	double *A, *AA, *C, *D, *CD, *R;
	double localmax, globalMax;

	A = (double*)malloc(sizeof(double)*N*N);
	At = (double*)malloc(sizeof(double)*N*N/P);

	AA = (double*)malloc(sizeof(double)*N*N);
	C = (double*)malloc(sizeof(double)*N*N);
	D = (double*)malloc(sizeof(double)*N*N);
	CD = (double*)malloc(sizeof(double)*N*N);
	R = (double*)malloc(sizeof(double)*N*N);
        
    //Inicialización de matrices
    for (int i=0; i<N*N; i++){
        A[i] = 1;
        C[i] = 1;
        D[i] = 1;
    }

    //Transposición + Máximo Local
    localmax = 0;
    MPI_Bcast(A, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i=0; i<N/P; i++){
		for (int j=0; j<N; j++){
			if (A[i*N+j] > localmax)
				localmax = A[i*N+j];
			At[j*N+i] = A[i*N+j];
		}
    }
    MPI_Allreduce(&localMax, &globalMax, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);



        
}

void slave(int rank, int P, int N)
{
	int i, j, k;
	double *A, *AA, *C, *D, *CD, *R;
	double localMax;
	double globalMax;

	A = (double*)malloc(sizeof(double)*N*N);
	At = (double*)malloc(sizeof(double)*N*N/P);

	AA = (double*)malloc(sizeof(double)*N*N/P);
	C = (double*)malloc(sizeof(double)*N*N/P);
	D = (double*)malloc(sizeof(double)*N*N);
	CD = (double*)malloc(sizeof(double)*N*N/P);
	R = (double*)malloc(sizeof(double)*N*N/P);	

	//Transposición + Máximo Local
    localmax = 0;
    MPI_Bcast(A, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i=0; i<N/P; i++){
		for (int j=0; j<N; j++){
			if (A[i*N+j] > localmax)
				localmax = A[i*N+j];
			At[j*N+i] = A[i*N+j];
		}
    }
    MPI_Allreduce(&localMax, &globalMax, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    

}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void imprimir(double *m)
{
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<N; j++)
		{
			printf("%.0f\t", m[i*N+j]);
		}
		printf("\n");
	}
}

int check(double *	X, int n)
{
    int ok = 1;
    for(int i=0; i<n; i++){
         for(int j=0; j<n; j++){
		  ok = ok && (R[i*n+j] == n*2*n);
   }
  }
  return ok;
}

void Multiply(double *X, double *Y, double *Z)
{
	double acc = 0;
	for (int i=0; i<N/P; i++) 
	{
		for (int j=0; j<N; j++) 
		{
			acc = 0;
			for (int k=0; k<N; k++)
			{
				acc += X[i*N+k] * Y[k+j*N]; 
			}
			Z[i*N+j] = acc; 
		}
	}

}
