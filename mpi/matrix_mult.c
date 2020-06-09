#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void master(int, int);
void slave(int, int);
double dwalltime(void);

int N = 1024; //tamaño
double *C, *D, *R;
int P; //cantidad de procesos

//R = avgCD*(AA + maxA*CD)

int main(int argc, char* argv[]){
    int myRank;

    MPI_Init(&argc, &argv);
    //Retorna el rank del proceso dentro del grupo comm (integer)
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    //Retorna cantidad de procesos en el grupo comm (integer)
    MPI_Comm_size(MPI_COMM_WORLD, &P);


    if(myRank == 0)
        master(myRank, P);
    else
        slave(myRank, P);

    MPI_Finalize();

    return(0);
}

void master(int id, int P){
    int i, j, k;
    int check = 1;

    C = (double*)malloc(sizeof(double)*N*N);
    D = (double*)malloc(sizeof(double)*N*N);
    R = (double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            C[i*N+j] = 1.0;
            D[i*N+j] = 1.0;
            R[i*N+j] = 0.0;
        }
    }


    MPI_Bcast(D, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //*senfbuf, sendcount, sendtype ,*recvbuf, recvcount, rcvtype, root, comm
    MPI_Scatter(C, N*N/P, MPI_DOUBLE, C, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Multiplicar
    for (i=0; i<N/P; i++)
    	for(j=0; j<N; j++)
    		for(k=0; k<N; k++)
    			R[i*N+j] = R[i*N+j] + C[i*N+k] * D[k+j*N];

	MPI_Gather(R, N*N/P, MPI_DOUBLE, R, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//Verifica el resultado
  	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			check=check&&(R[i*N+j]==N);
  		 }
  	}

	if(check)
		printf("Multiplicacion de matrices resultado correcto\n");
	else
		printf("Multiplicacion de matrices resultado erroneo\n");


}

void slave(int id, int cantidadProcesos){
	int i, j, k;
	C = (double*)malloc(sizeof(double)*N*N/cantidadProcesos);
    D = (double*)malloc(sizeof(double)*N*N/cantidadProcesos);
    R = (double*)malloc(sizeof(double)*N*N/cantidadProcesos);

    //Multiplicar
    for (i=0; i<N/P; i++)
    	for(j=0; j<N; j++)
    		for(k=0; k<N; k++)
    			R[i*N+j] = R[i*N+j] + C[i*N+k] * D[k+j*N];

    //Retornar
    MPI_Gather(R, N*N/P, MPI_DOUBLE, R, N*N/P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

}

/*
    timetick = dwalltime();
    ...
    printf("Tiempo en segundos %f \n", dwalltime() - timetick);
*/
#include<sys/time.h>
double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

