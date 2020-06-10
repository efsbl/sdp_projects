//Resolver la expresion R = avgCD * (AA + maxA * CD)
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

void master(void);
void slave(void);
void imprimir(double *m);
int check(double *A, int n);
double dwalltime(void);
void Multiply(double *X, double *Y, double *Z);

unsigned int N; 
unsigned int max = 0;
unsigned int localmax = 1;
double sum = 0;
double localsum = 0;
double prom;
int rank;
MPI_Status *status;
MPI_Request *request;
int P;
double *A;
double *At;
double *AAt;
double *C;
double *D;
double *CD;

double *R;



int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	N = atoi(argv[1]);

	if (rank == 0)
		master();
	else
		slave();

	MPI_Finalize();
	return 0;
}

void master()
{
	A = (double*)malloc(sizeof(double) * N * N);
	At = (double*)malloc(sizeof(double) * N * N);
	C = (double*)malloc(sizeof(double) * N * N);
	D = (double*)malloc(sizeof(double) * N * N);
	R = (double*)malloc(sizeof(double) * N * N);
	AAt = (double*)malloc(sizeof(double) * N * N);
	CD = (double*)malloc(sizeof(double) * N * N);

	for (int i = 0; i < N * N; i++)
	{
		A[i] = 1;
		At[i] = 1;
		C[i] = 1;
		D[i] = 1;

	}
    //imprimir(A);
	double start = dwalltime();
	//calculo maximo de A
    MPI_Scatter(A, N * N/ P, MPI_UNSIGNED, A, N * N / P, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	for (int i = 0; i < N/P; i++){
		for (int j = 0; j < N; j++){
		  if (A[i * N + j] > localmax)
			localmax = A[i * N + j];
		}
    }	
	MPI_Reduce(&localmax, &max, 1, MPI_UNSIGNED, MPI_MAX, 0, MPI_COMM_WORLD);

	//multiplico A*At
	MPI_Bcast(At, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, N * N / P, MPI_DOUBLE, A, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	Multiply(A, At, AAt);
	MPI_Gather(AAt, N * N / P, MPI_DOUBLE, AAt, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	//imprimir(AAt);
	
    //multiplico C*D
	MPI_Bcast(D, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(C, N * N / P, MPI_DOUBLE, C, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	Multiply(C, D, CD);
	
	MPI_Gather(CD, N * N / P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	//imprimir(CD);

	//calculo avgCD
    MPI_Scatter(CD, N * N/ P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	for (int i = 0; i < N/P; i++){
		for (int j = 0; j < N; j++){
			localsum = localsum + CD[i * N + j];
	    }
	}
	MPI_Reduce(&localsum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	prom=sum/(N*N);
	
	//operacion final
	
	MPI_Bcast(&prom, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&max, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	MPI_Scatter(AAt, N * N/ P, MPI_DOUBLE, AAt, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(CD, N * N/ P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			R[i * N + j] = ((AAt[i * N + j] + ((CD[i * N + j])*max))*prom);	
		}	
	}	
	
	MPI_Gather(R, N * N / P, MPI_DOUBLE, R, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
    if (check(R,N))
		printf("Resultado Correcto\n");
	else
		printf("Resultado Incorrecto\n");
	
	printf("Tiempo: %f\n", dwalltime() - start);
	printf("N: %d", N);
	printf("\n");
	printf("P: %d", P);
	printf("\n");
	printf("Max: %d ", max);
	printf("\n");
	printf("Promedio: %f ", prom);
	printf("\n");
	//imprimir(R);
}

void slave()
{
	A = (double*)malloc(sizeof(double) * N * N / P);
    At = (double*)malloc(sizeof(double) * N * N);
	C = (double*)malloc(sizeof(double) * N * N / P);
	D = (double*)malloc(sizeof(double) * N * N);
	AAt = (double*)malloc(sizeof(double) * N * N / P);
	CD = (double*)malloc(sizeof(double) * N * N / P);
	R = (double*)malloc(sizeof(double) * N * N / P);

    MPI_Scatter(A, N * N/ P, MPI_UNSIGNED, A, N * N / P, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	for (int i = 0; i < N/P; i++)
		for (int j = 0; j < N; j++){
		  if (A[i * N + j] > localmax)
			localmax = A[i * N + j];
	}
	MPI_Reduce(&localmax, &max, 1, MPI_UNSIGNED, MPI_MAX, 0, MPI_COMM_WORLD);



	MPI_Bcast(At, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, N * N / P, MPI_DOUBLE, A, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	Multiply(A, At, AAt);
	MPI_Gather(AAt, N * N / P, MPI_DOUBLE, AAt, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	MPI_Bcast(D, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(C, N * N / P, MPI_DOUBLE, C, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	Multiply(C, D, CD);
	MPI_Gather(CD, N * N / P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  

    MPI_Scatter(CD, N * N/ P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	for (int i = 0; i < N/P; i++)
		for (int j = 0; j < N; j++){
			localsum = localsum + CD[i * N + j];
	}
	MPI_Reduce(&localsum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  	MPI_Bcast(&prom, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&max, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	MPI_Scatter(AAt, N * N/ P, MPI_DOUBLE, AAt, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(CD, N * N/ P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	for (int i = 0; i < N/P; i++){
		for (int j = 0; j < N; j++){
			R[i * N + j] = ((AAt[i * N + j] + ((CD[i * N + j])*max))*prom);
		}	
	}	
	MPI_Gather(R, N * N / P, MPI_DOUBLE, R, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
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
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%.0f\t", m[i * N + j]);
		}
		printf("\n");
	}
}

int check(double *	X, int n)
{
    int ok = 1;
    for(int i=0;i<n;i++){
         for(int j=0;j<n;j++){
		  ok=ok&&(R[i*n+j]==n*2*n);
   }
  }
  return ok;
}

void Multiply(double *X, double *Y, double *Z)
{
	double acc = 0;
	for (int i = 0; i < N / P; i++) 
	{
		for (int j = 0; j < N; j++) 
		{
			acc = 0;
			for (int k = 0; k < N; k++)
			{
				acc += X[i * N + k] * Y[k + j * N]; 
			}
			Z[i * N + j] = acc; 
		}
	}
}