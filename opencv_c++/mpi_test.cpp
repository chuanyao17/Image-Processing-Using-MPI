#include <iostream>
#include "mpi.h"

using namespace std;

int main(int argc, char** argv) 
{
    int p, id;
    MPI_Init( &argc, &argv );
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Status status;
    int input;
    if (id==1)
    {
        cin>>input;
    }
    
    cout<<p<<" "<<id<<" "<<input<<endl;
    MPI_Finalize();
    return 0;

}