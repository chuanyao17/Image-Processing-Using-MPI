#include <stdio.h>
#include "mpi.h"


int main(int argc, char** argv) 
{
    int p, id;
    MPI_Init( &argc, &argv );
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Status status;

    int input;
    

    printf("%d %d\n",p,id);

    scanf("%d",&input);

    printf("$$$ %d %d\n",id,input);

    
    

    MPI_Finalize();
    return 0;

}