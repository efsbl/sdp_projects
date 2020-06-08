#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void master(int, int);
void slave(int, int);
double dwalltime(void);

int N = 1024;
double *A, *B, *C;

int main(int argc, char* argv[]){
    int myRank;
    int cantidadProcesos;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidadProcesos);


    if(myRank == 0)
        master(myRank, cantidadProcesos);
    else
        slave(myRank, cantidadProcesos);

    MPI_Finalize();
    return(0);
}

void master(int id, int cantidadProcesos){
    int i, j;

    A = (double*)malloc(sizeof(double)*N*N);
    B = (double*)malloc(sizeof(double)*N*N);
    C = (double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            A[i*N+j] = 1.0;
            B[i*N+j] = 1.0;
            C[i*N+j] = 0.0;
        }
    }



}

void slave(int id, int cantidadProcesos){

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
