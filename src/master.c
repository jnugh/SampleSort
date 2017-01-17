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
  int bucketBoundariesBuffer[(world_size - 1) * getSampleSizeEach() * sizeof(int)];
  for(int i = 1; i < world_size; i++) {
    MPI_Recv(&bucketBoundariesBuffer[(i - 1) * getSampleSizeEach()], getSampleSizeEach(), MPI_INT, i, MPI_TAG_BOUNDARY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  qsort(bucketBoundariesBuffer, (world_size - 1) * getSampleSizeEach(), sizeof(int), cmpfunc);

  int *bucketBoundaries = malloc((world_size - 1) * sizeof(int));
  int stepSize = (world_size - 1) * getSampleSizeEach() / world_size;
  for(int i = 0; i < world_size - 1; i++) {
    bucketBoundaries[i] = bucketBoundariesBuffer[(i + 1) * stepSize];
  }

  stopTimer("Sort sample");
  return bucketBoundaries;
}

void distributeBucketInformation(int *bucketBoundaries, int world_size){
  int i;
  startTimer();
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(bucketBoundaries, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);

  for(i = 1; i < world_size; i++) {
    //Synchronize sender information
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }

  i = -1;
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);

  stopTimer("Distribute to buckets");
}

void gatherResults() {
  startTimer();
  MPI_Barrier(MPI_COMM_WORLD);
  stopTimer("SortLocally all");
}

int fetchResults(int **data, int world_size) {
  startTimer();
  int sizes[world_size];
  int sizeSum = 0;
  for(int i = 1; i < world_size; i++) {
    MPI_Recv(&(sizes[i-1]), 1, MPI_INT, i, MPI_TAG_RETURN_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    sizeSum += sizes[i-1];
  }
  *data = malloc(sizeSum * sizeof(int));
  int pos = 0;
  for(int i = 1; i < world_size; i++) {
    MPI_Recv(&((*data)[pos]), sizes[i-1], MPI_INT, i, MPI_TAG_RETURN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    pos += sizes[i-1];
  }
  stopTimer("FetchResults");
  return sizeSum;
}
