#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void printMatrix(double* , int);

int main(int argc, char*argv[]){

	double *A, *At;
	int i, j, k, N;
	
	if (argc < 2){
   		printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
   		return 0;
  	}

  	N = atoi(argv[1]);


	A=(double*)malloc(sizeof(double)*N*N);
	At=(double*)malloc(sizeof(double)*N*N);

	//Matriz triangular inferior
	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			if (i>=j)
				A[i*N+j]= 1.0;
			else
				A[i*N+j]= 0.0;
   		}
  	}

  	printMatrix(A, N);

  	//transpose
  	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			At[i*N+j] = A[j*N+i];
   		}
  	}

  	printf("\n\n");

  	printMatrix(At, N);

	return 0;
}

void printMatrix(double* A, int N){
	int i, j;
	for(i=0; i<N; i++){
		for(j=0; j<N; j++) {
			printf("%.0f ", A[i*N+j]);
		}
		printf("\n");
	}
}
