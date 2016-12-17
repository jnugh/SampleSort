#include "master.h"
#include "helpers.h"
#include "timer.h"
#include "mpi.h"
#include "mpi_tags.h"
#include <stdio.h>
#include <stdlib.h>

void readAndDistribute(const char *inputFile, int world_size){
  int *rawData;
  int dataLen = readData(inputFile, &rawData);
  int distributionSize = dataLen / (world_size - 1);

  startTimer();
  for(int i = 1; i < world_size; i++) {
    int sendSize = distributionSize;
    if(i == world_size - 1) {
      sendSize = dataLen - (i - 1) * distributionSize;
    }
    MPI_Send(&sendSize, 1, MPI_INT, i, MPI_TAG_DATALEN, MPI_COMM_WORLD);

    MPI_Send(&rawData[(i - 1) * distributionSize], sendSize, MPI_INT, i, MPI_TAG_DATA, MPI_COMM_WORLD);
  }
  stopTimer("Distribute");
}

int *sortSample(int world_size){
  startTimer();
  int *bucketBoundaries = malloc((world_size - 1) * sizeof(int));
  for(int i = 1; i < world_size; i++) {
    MPI_Recv(&bucketBoundaries[i - 1], 1, MPI_INT, i, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  qsort(bucketBoundaries, world_size - 1, sizeof(int), cmpfunc);
  stopTimer("Sort sample");
  return bucketBoundaries;
}

void distributeBucketInformation(int *bucketBoundaries, int world_size){
  startTimer();
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(bucketBoundaries, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);

  for(int i = 1; i < world_size; i++) {
    //Synchronize sender information
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for(int i = 1; i < world_size; i++) {
      int status = 0;
      MPI_Recv(&status, 1, MPI_INT, i, MPI_TAG_SENDERSTATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for(int i = 1; i < world_size; i++) {
      int status = 0;
      MPI_Send(&status, 1, MPI_INT, i, MPI_TAG_SENDERSTATUS, MPI_COMM_WORLD);
    }
  }

  stopTimer("Distribute to buckets");
}
