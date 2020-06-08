#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void printMatrix(double* , int);

double *A, *At, *C;
int N, T;

void *funcion(void *arg){
	int i, j;
	int tid = *(int*)arg;
	int desde = tid*N/T;
	int hasta = (tid+1)*N/T;
	for(i=desde; i<hasta; i++){
		for(j=0; j<N; j++){
			At[j*N+i] = A[i*N+j];
		}
	}
	pthread_exit(NULL);
}


int main(int argc, char*argv[]){

	int i, j, k;

	if (argc < 3){
   		printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
   		return 0;
  	}

  	N = atoi(argv[1]);
  	T = atoi(argv[2]);

	A=(double*)malloc(sizeof(double)*N*N);
	At=(double*)malloc(sizeof(double)*N*N);\

	pthread_t misThreads[T];
	int threads_ids[T];

	//Inicializacion matrix diagonal inferior
	/*for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			if (i>=j)
				A[i*N+j]= 1.0;
			else
				A[i*N+j]= 0.0;
   		}
  	}*/

	int value = 0;
  	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			A[i*N+j] = value++;
   		}
  	}

  	printf("\n");
  	printMatrix(A, N);

  	printf("\n\n");

 	for(int id=0; id<T; id++){
        threads_ids[id] = id;
        pthread_create(&misThreads[id], NULL, &funcion, (void*)&threads_ids[id]);
    }

    printMatrix(At, N);



  	free(A);
  	free(At);
	return 0;
}

void printMatrix(double* A, int N){
	int i, j;
	for(i=0; i<N; i++){
		for(j=0; j<N; j++) {
			printf("%.0f\t", A[i*N+j]);
		}
		printf("\n");
	}
}
