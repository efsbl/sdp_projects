#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void initMatrix(double*, int);
void printMatrix(double*, int);
void transposeMatrix(double*, int);
double dwalltime(void);

double *A, *At, *C;
int N, T;
int mode = 0; // if mode = 0 transpose, if mode = 1 mult.

//Funcion que realizan los threads
void *threadFunc(void *arg){
	int tid = *(int*)arg;
	int i, j, k;
	int hasta = (tid+1)*N/T;
	if (mode == 0){	
		//Transposicion	
		for(i=tid*N/T; i<hasta; i++){
			for(j=i+1; j<N; j++) {
				At[j*N+i] = A[i*N+j];
			}
		
		}
	}else{
		//Multiplicacion	
		for(i=tid*N/T; i<hasta; i++){
			for(j=0; j<N; j++){
				C[i*N+j] = 0;
				for(k=0; k<N; k++){
					C[i*N+j] = C[i*N+j] + A[i*N+k] * At[k+j*N];
				}
			}
		}	
	}
	pthread_exit(NULL);	
}

int main(int argc, char*argv[]){
	double timetick, temp;
	int i, j, k;
	int check = 1;

	if (argc < 3){
		printf("\n Faltan argumentos: N dimension de la matriz, T numero de threads \n");
		return 0;
	}

	N=atoi(argv[1]);
	T=atoi(argv[2]);
	A=(double*)malloc(sizeof(double)*N*N);
	At=(double*)malloc(sizeof(double)*N*N);
	C=(double*)malloc(sizeof(double)*N*N);	
	
	//Inicializa la matriz A en 1, el resultado sera una matriz con todos sus valores en N
	initMatrix(A, N);
	initMatrix(At, N);

	//Multiplicacion secuencial	
	//for (i=0; i<N; i++){
	//	for (j=0; j<N; j++){
	//		C[i*N+j]=0;
	//		for (k=0; k<N; k++){
	//			C[i*N+j] = C[i*N+j] + A[i*N+k] * At[k+j*N];
	//		}
	//	}
	//}	

	
	pthread_t misThreads[T];
	int threads_ids[T];
	
	for(int id=0; id<T; id++){
		threads_ids[id] = id;
		pthread_create(&misThreads[id], NULL, &threadFunc, (void*)&threads_ids[id]);
	}
	
	//Trasposicion
	for(int id=0; id<T; id++){
		pthread_join(misThreads[id], NULL);
	}

	mode = 1;
	//Multiplicacion
	
	for(int id=0; id<T; id++){
		pthread_join(misThreads[id], NULL);
	}

	//Verifica el resultado
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			check=check&&(C[i*N+j]==N);
		}
	}

	if(check)
		printf("Multiplicacion de matrices resultado correcto\n");
	else
		printf("Multiplicacion de matrices resultado erroneo\n");


	free(A);	
	free(At);
	free(C);

	return 0;


}

void initMatrix(double *A, int N){
	int i, j;
	for (i=0; i<N; i++){
		for(j=0; j<N; j++){
			A[i*N+j] = 1;
		}
	}
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

//Traspuesta secuencial
void transposeMatrix(double* A, int N){
	int i, j;
	double temp;
	for(i=0; i<N; i++){
		for(j=i+1; j<N; j++) {
			temp = A[i*N+j];
			A[i*N+j] = A[j*N+i];
			A[j*N+i] = temp;
		}
	
	}
}

#include<sys/time.h>
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

