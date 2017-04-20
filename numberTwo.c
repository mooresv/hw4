// Adapted from http://mpitutorial.com/tutorials/introduction-to-groups-and-communicators/

#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    // Get the rank and size in the original communicator
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the group of processes in MPI_COMM_WORLD
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int a[world_size];
    int n, i, result, numPrimes;

    n = 1;
    i = 0;
    numPrimes = 0;
    while( n < world_size ) {
        if( isPrime(n) ) {
            a[i] = n;
            numPrimes++;
            i++;
        }
        n++;
    }

    int primeArray[numPrimes];
    i=0;
    while( i < numPrimes ) {
        primeArray[i] = a[i];
        i++;
    }

    // Construct a group containing all of the prime ranks in world_group
    MPI_Group prime_group;
    MPI_Group_incl(world_group, numPrimes, primeArray, &prime_group);

    // Create a new communicator based on the group
    MPI_Comm prime_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

    int prime_rank = -1, prime_size = -1;
    if (MPI_COMM_NULL != prime_comm) {
        MPI_Comm_rank(prime_comm, &prime_rank);
        MPI_Comm_size(prime_comm, &prime_size);
    }

    if( world_rank == 0 ) {
        printf("Prime size: %d\n", numPrimes);
    } else if( prime_rank != -1 ) {
        printf("World rank: %d, Prime rank: %d\n", world_rank, (prime_rank));
    }

    MPI_Finalize();
}

int isPrime(int number) {
    int i;
    for ( i = 2; i*i <= number; i++) {
        if (number % i == 0) return 0;
    }
    return 1;
}