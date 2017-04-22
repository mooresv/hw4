/* test driver program for 1D matrix transpose */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

extern int transpose1d(int *a, int n, int blockdim, MPI_Comm comm);

int main (int argc, char *argv[]) {
    int   numtasks, taskid;
    int i, j, n, nlocal; 
    int *alocal;
    int offset;
    int checkrank = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    if (argc != 2) {
        if (taskid == 0) {
            fprintf(stderr, "Usage: %s <n>\n", argv[0]);
            fprintf(stderr, "where n is a multiple of the number of tasks.\n");
        }   
        MPI_Finalize();
        exit(0);
    }

    /* Read row/column dimension from command line */
    n = atoi(argv[1]);

    if (n%numtasks) {
        if (taskid == 0) {
            fprintf(stderr, "Usage: %s <n>\n", argv[0]);
            fprintf(stderr, "where n is a multiple of the number of tasks.\n");
        }   
        MPI_Finalize();
        exit(0);
    }

    nlocal = n/numtasks;
    // nlocal = n;

    /* Allocate local row block */
    alocal = (int *) malloc(n*nlocal*sizeof(int));

    /* Initialize local block */
    offset = taskid*n*nlocal;
    for (i = 0; i < nlocal; i++)
        for (j = 0; j < n; j++)
            alocal[i*n + j] = offset + i*n + j + 1;

    if (taskid == checkrank) {
        printf("process %d local block before transpose\n", taskid);
        for (i = 0; i < nlocal; i++) {
            for (j = 0; j < n; j++) 
                    printf("%10d ", alocal[i*n + j]);
            printf("\n");
        }
    }

    transpose1d(alocal, n, nlocal, MPI_COMM_WORLD);

    if (taskid == checkrank) {
        printf("process %d local block after transpose\n", taskid);
        for (i = 0; i < nlocal; i++) {
            for (j = 0; j < n; j++) 
                    printf("%10d ", alocal[i*n + j]);
            printf("\n");
        }
        // for (i = 0; i < n; i++) {
        //     for (j = 0; j < nlocal; j++) 
        //             printf("%10d ", alocal[i*nlocal + j]);
        //     printf("\n");
        // }

        // printf("\n\n%d", nlocal);
        // printf("\n%d\n\n", n);
    }

    MPI_Finalize();
    return(0);
}

int transpose1d(int *a, int n, int blockdim, MPI_Comm comm) {

    int numtasks, taskid;
    int temp = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Request request;
    MPI_Status status;


    // Scatter data
    int *alocal = (int *) malloc(n*blockdim*sizeof(int));
    // Scatter in parts
    int i, j, k;
    for(i = 0; i < blockdim; i++ ) {
        for(j = 0; j < n; j+= blockdim ) {
            for( k = 0; k < blockdim; k++ ) {
                MPI_SEND(a + (i*blockdim + j + k), 1, MPI_INT, j*blockdim, 0, comm);
            }
        }
    }
    
    // just a check to see if it worked
    // if( taskid == 1 ) {
    //     for (i = 0; i < blockdim; i++) {
    //         for (j = 0; j < n; j++)
    //             printf(" i: %d", alocal[i*n + j]);
    //         printf("\n");
    //     }
    // }

    // transpose arrays
    // for(i = 0; i < blockdim; i++ ) {
    //     for(j = 0; j < n; j+= blockdim ) {
    //         a[i*n + j] = alocal[i*n + j];
    //     }
    // }

    // if( taskid == 1 ) {
    //     for (i = 0; i < blockdim; i++) {
    //         for (j = 0; j < n; j++)
    //             printf(" i: %d", a[i*n + j]);
    //         printf("\n");
    //     }
    // }

    // send out finished array
    // MPI_Gather( a, int send_count, MPI_Datatype send_datatype, 
    //     void* recv_data, int recv_count, MPI_Datatype recv_datatype, int root, MPI_Comm communicator);

    // printf("HI %d %d %d %d\n", n, blockdim, numtasks, taskid);
    return(0);
}
 

