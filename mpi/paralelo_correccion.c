
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

//Resolver la expresion R = avgCD * (AA + maxA * CD)

void master(int, int, int);
void slave(int, int, int);
int check(double *, int);
double dwalltime(void);
void Multiply(double *, double *, double *, int, int);

int main(int argc, char *argv[])
{
	unsigned int N; //Matrices de tamaño NxN
	int rank; //Rank o Id de cada proceso dentro del Communicator
	int P; //Número de procesos

    if (argc < 2){
        printf("Falta argumeno N (matrices de N*N)\n");
        return 0;
    }

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
	double *A, *At, *AA, *C, *D, *CD, *R;
	double localMax, globalMax;
    double localSum, globalSum;
    double avg;

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

    double start = dwalltime();

    //Transposición + Máximo Local
    localMax = 0;
    MPI_Bcast(A, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (i=0; i<N/P; i++){
		for (j=0; j<N; j++){
			if (A[i*N+j] > localMax)
				localMax = A[i*N+j];
			At[j*N+i] = A[i*N+j];
		}
    }
    MPI_Allreduce(&localMax, &globalMax, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    //Multiplicacion A*At
    Multiply(A, At, AA, N, P);

    //Multiplicacion C*D
    MPI_Bcast(D, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(C, N*N/P, MPI_DOUBLE, C, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    Multiply(C, D, CD, N, P);

    //Suma valores matriz CD y cálculo del promedio
    localSum = 0;
    for(i=0; i<N/P; i++){
        for(j=0; j<N; j++){
            localSum = localSum + CD[i*N+j];
        }
    }
    MPI_Allreduce(&localSum, &globalSum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    avg = globalSum/(N*N);

    //R = avgCD * (AA + maxA*CD)
    for(i=0; i<N/P; i++){
        for(j=0; j<N; j++){
            R[i*N+j] = avg*(AA[i*N+j] + globalMax*(CD[i*N+j]));
        }
    }
    MPI_Gather(R, N*N/P, MPI_DOUBLE, R, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Chequeo de resultado (utilizando matrices en '1' debe dar N*2*N)
    if (check(R,N))
        printf("Resultado Correcto\n");
    else
        printf("Resultado Incorrecto\n");

    printf("Tiempo: %f\n", dwalltime() - start);
    printf("N: %d\n", N);
    printf("P: %d\n", P);
    printf("Max: %f\n", globalMax);
    printf("Promedio: %f\n", avg);

}

void slave(int rank, int P, int N)
{
	int i, j, k;
	double *A, *At, *AA, *C, *D, *CD, *R;
	double localMax, globalMax;
    double localSum, globalSum;
    double avg;

	A = (double*)malloc(sizeof(double)*N*N);
	At = (double*)malloc(sizeof(double)*N*N/P);
	AA = (double*)malloc(sizeof(double)*N*N/P);
	C = (double*)malloc(sizeof(double)*N*N/P);
	D = (double*)malloc(sizeof(double)*N*N);
	CD = (double*)malloc(sizeof(double)*N*N/P);
	R = (double*)malloc(sizeof(double)*N*N/P);

	//Transposición + Máximo Local
    localMax = 0;
    MPI_Bcast(A, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (i=0; i<N/P; i++){
		for (j=0; j<N; j++){
			if (A[i*N+j] > localMax)
				localMax = A[i*N+j];
			At[j*N+i] = A[i*N+j];
		}
    }
    MPI_Allreduce(&localMax, &globalMax, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    //Multiplicacion A*At
    Multiply(A, At, AA, N, P);

    //Multiplicacion C*D
    MPI_Bcast(D, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(C, N*N/P, MPI_DOUBLE, C, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    Multiply(C, D, CD, N, P);

    //Suma valores matriz CD y cálculo del promedio
    localSum = 0;
    for(i=0; i<N/P; i++){
        for(j=0; j<N; j++){
            localSum = localSum + CD[i*N+j];
        }
    }
    MPI_Allreduce(&localSum, &globalSum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    avg = globalSum/(N*N);

    //R = avgCD * (AA + maxA*CD)
    for(i=0; i<N/P; i++){
        for(j=0; j<N; j++){
            R[i*N+j] = avg*(AA[i*N+j] + globalMax*(CD[i*N+j]));
        }
    }
    MPI_Gather(R, N*N/P, MPI_DOUBLE, R, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

int check(double *R, int N)
{
    int ok = 1;
    for(int i=0; i<N; i++){
         for(int j=0; j<N; j++){
		  ok = ok && (R[i*N+j] == N*2*N);
   }
  }
  return ok;
}

void Multiply(double *X, double *Y, double *Z, int N, int P)
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
