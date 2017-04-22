/* test driver program for 2D block matrix transpose */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern int transpose2d(int *a, int blocksize, MPI_Comm comm2d);

int main (int argc, char *argv[])
{
   int ROW = 0, COL = 1;  /* for readability */  
  int   numtasks, taskid;
  int i, j, n, nlocal; 
  int *alocal;
  int dims[2], periods[2];;
  MPI_Comm comm2d;
  int my2drank;
  int nrowblocks;
  int mycoords[2];
  int checkrank = 4;
  int offset;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  if (argc != 2) {
    if (taskid == 0) {
      fprintf(stderr, "Usage: %s <n>\n", argv[0]);
      fprintf(stderr, "where n is a multiple of the square root of the number of tasks\n");
    }   
    MPI_Finalize();
    exit(0);
  }

  /* Read row/column dimension from command line */
  n = atoi(argv[1]);

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  dims[ROW] = dims[COL] = sqrt(numtasks);

  if (n%dims[ROW] != 0) {
    if (taskid == 0) {
      fprintf(stderr, "Usage: %s <n>\n", argv[0]);
      fprintf(stderr, "where n is a multiple of the square root of the number of tasks\n");
    }   
    MPI_Finalize();
    exit(0);
  }

  nlocal = n/dims[ROW];

  /* Create 2D Cartesian communicator */
  periods[ROW] = periods[COL] = 0;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm2d);
  MPI_Comm_rank(comm2d, &my2drank);
  MPI_Cart_coords(comm2d, my2drank, 2, mycoords);

  /*  Allocate memory for local block of A */
  alocal = (int *) malloc(nlocal*nlocal*sizeof(int));

  /*  Initialize local matrix block */
  offset = mycoords[0]*n*nlocal + mycoords[1]*nlocal + 1;
  for (i = 0; i < nlocal; i++)
      for (j = 0; j < nlocal; j++)
          alocal[i*nlocal+j] = offset + i*n + j;

  if (my2drank==checkrank) {
      printf("\noriginal matrix block for process %d\n", my2drank);
      for (i = 0; i < nlocal; i++) {
          for (j = 0; j < nlocal; j++)
              printf("%10d ", alocal[i*nlocal+j]);
          printf("\n");
      }
  }

  transpose2d(alocal, nlocal, comm2d);

  if (my2drank==checkrank) {
      printf("\nlocal transpose for process %d\n", my2drank);
      for (i = 0; i < nlocal; i++) {
          for (j = 0; j < nlocal; j++)
              printf("%10d ", alocal[i*nlocal+j]);
          printf("\n");
      }
  }

  MPI_Finalize();
  exit(0);
}

int transpose2d(int *a, int blockdim, MPI_Comm comm2d) 
{
    int my2drank, mycoords[2], temp;
    MPI_Comm_rank(comm2d, &my2drank);
    MPI_Cart_coords(comm2d, my2drank, 2, mycoords);
    MPI_Request request;
    MPI_Status status;
    // printf("Test: %d %d %d %d\n", my2drank, mycoords[0], mycoords[1], blockdim);

    // // local transpose
    int j, k;
    for( j = 0; j < blockdim; j++ ) {
        for( k = 0; k < j; k++ ) {
            temp = a[j*blockdim + k];
            a[j*blockdim + k] = a[k*blockdim + j];
            a[k*blockdim + j] = temp;
        }
    }

    if( mycoords[0] != mycoords[1] ) {
        // send item by item to their necessary companion
        for( j = 0; j < blockdim; j++ ) {
            for( k = 0; k < blockdim; k++ ) {
                // index = i*n + j
                // printf("%d: %d %d %d %d\n", taskid, j/blockdim, 0, j*blockdim, a[i*blockdim + j]);
                MPI_Irecv( &temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm2d, &request);
                MPI_Send(a + (j*blockdim + k), 1, MPI_INT, mycoords[1] * blockdim + mycoords[0], 0, comm2d);
                // printf("%d: sent \n", taskid);
                // printf("%d: waiting to recieve %d\n", taskid, count);
                MPI_Wait( &request, &status );
                // printf("%d: recieved from %d\n", my2drank, status.MPI_SOURCE);
                a[j*blockdim + k] = temp;
            }
        }
    }


    // int *alocal = (int *) malloc(blockdim*blockdim*sizeof(int));


    // MPI_Comm_rank(comm2d, &my2drank);
    // MPI_Cart_coords(comm2d, my2drank, 2, mycoords);
    // printf("Test: %d %d %d %d\n", my2drank, mycoords[0], mycoords[1], blockdim);

    // // Now transpose the local array
    // int i, j, temp;
    // for( i = 0; i < blockdim; i++ ) {
    //     for( j = 0; j < blockdim; j++ ) {

    //         alocal[i*blockdim + j] = a[i*blockdim + j];
    //     }
    // }
    // for( i = 0; i < blockdim; i++ ) {
    //     for( j = 0; j < blockdim; j++ ) {

    //         a[i*blockdim + j] = alocal[j*blockdim + i];
    //     }
    // }

    // return(0);
}
