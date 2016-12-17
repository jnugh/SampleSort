#include "mpi.h"
#include "samplesort.h"
#include "master.h"
#include "client.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int world_rank;
int world_size;

int main (int argc, char *argv[]) {
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  srand(time(NULL));

  if(world_rank == 0) {
    if(argc != 3) {
      showHelp();
      exit(1);
    }
    readAndDistribute(argv[1], world_size);
    int *bucketBoundaries = sortSample(world_size);
    distributeBucketInformation(bucketBoundaries, world_size);
    free(bucketBoundaries);
  } else {
    int *localData;
    int dataSize = collectInputData(&localData);
    int sample = selectSample(localData, dataSize);
    sendSample(sample);
    distributeToCorrectBuckets(&localData, dataSize, world_size, world_rank);
  }

  MPI_Finalize();
  return 0;
}

void showHelp() {
  printf("Please specify input and output location as commandline options\n");
}