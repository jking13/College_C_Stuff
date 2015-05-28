#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define NREPS 10
#define NMAX  524288
#define Min(a, b) (a > b ? b : a)

int main(int argc, char *argv[])
{
   int myid, numprocs;
   int  i, k, count;
   int *buf;
   double starttime=0, endtime=0, delta=0, mintime=0, realmintime=0;
   MPI_Status status;

   buf = (int *) malloc (NMAX * sizeof (int));
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD,&myid);

   count = 1;
   for (k = 0; k<20; k++) {
     mintime = 1000.;
     for (i=0; i<NREPS; i++) 
       if (myid == 0) {
         starttime = MPI_Wtime();
         MPI_Send (buf, count, MPI_INT, 1, 0, MPI_COMM_WORLD);
         MPI_Recv (buf, count, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
         endtime   = MPI_Wtime();
         delta = 0.5*(endtime - starttime);
         mintime = Min(mintime,delta);
//       printf("%e\n", delta);
       }
       else if (myid == 2) {
           starttime = MPI_Wtime();
           MPI_Send (buf, count, MPI_INT, 3, 0, MPI_COMM_WORLD);
           MPI_Recv (buf, count, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
           endtime   = MPI_Wtime();
           delta = 0.5*(endtime - starttime);
           mintime = Min(mintime,delta);
           //       printf("%e\n", delta);
       }
       else if (myid == 1){
         MPI_Recv (buf, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
         MPI_Send (buf, count, MPI_INT, 0, 0, MPI_COMM_WORLD);
       }
       else if (myid == 3){
           MPI_Recv (buf, count, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
           MPI_Send (buf, count, MPI_INT, 2, 0, MPI_COMM_WORLD);
       }
       MPI_Barrier(MPI_COMM_WORLD);
       MPI_Reduce(&mintime, &realmintime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
     if (myid==0) printf("%8d  %7.3e  %7.3e\n", count, mintime, (8.0*count)/(mintime*1024.*1024.));
     count = count*2;
   }

   MPI_Finalize();
}
