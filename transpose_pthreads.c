#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void initMatrix(double *, int);
void printMatrix(double *, int);
double dwalltime(void);

double *A, *At, *C;
int N, T;
int mode = 0; // mode = 0 -> transpose ; mode = 1 -> multiply

void *funcion(void *arg){
	int i, j, k;
	int tid = *(int*)arg;
	int desde = tid*N/T;
	int hasta = (tid+1)*N/T;
	if(!mode){
		for(i=desde; i<hasta; i++){
			for(j=0; j<N; j++){
				At[j*N+i] = A[i*N+j];
			}
		}
	}else{
		for(i=desde; i<hasta; i++){
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


int main(int argc, char *argv[]){

	int i, j, k;
	int check = 1;
	double timetick;

	if (argc < 3){
   		printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
   		return 0;
  	}

  	N = atoi(argv[1]);
  	T = atoi(argv[2]);

	A=(double*)malloc(sizeof(double)*N*N);
	At=(double*)malloc(sizeof(double)*N*N);
	C=(double*)malloc(sizeof(double)*N*N);

	pthread_t misThreads[T];
	int threads_ids[T];

	initMatrix(A, N);

 	for(int id=0; id<T; id++){
        threads_ids[id] = id;
        pthread_create(&misThreads[id], NULL, &funcion, (void*)&threads_ids[id]);
    }


    /* Transposicion + Multiplicacion */
    timetick = dwalltime();
    //Trasposicion
	for(int id=0; id<T; id++){
		pthread_join(misThreads[id], NULL);
	}

	mode = 1;
	//Multiplicacion

	for(int id=0; id<T; id++){
		pthread_join(misThreads[id], NULL);
	}

	printf("Tiempo en segundos %f \n", dwalltime() - timetick);

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
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			A[i*N+j] = 1.0;
		}
	}
}

void printMatrix(double *A, int N){
	int i, j;
	for(i=0; i<N; i++){
		for(j=0; j<N; j++) {
			printf("%.0f\t", A[i*N+j]);
		}
		printf("\n");
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
