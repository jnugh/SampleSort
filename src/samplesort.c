#include "mpi.h"
#include "samplesort.h"
#include "master.h"
#include "client.h"
#include "timer.h"
#include "helpers.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int world_rank;
int world_size;

int main (int argc, char *argv[]) {
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  srand(time(NULL));

  if(argc > 3) {
    setSampleSizeEach(atoi(argv[3]));
  }

  if(argc > 4) {
    if(strcmp(argv[4], "csv") == 0) {
      setTimerMode(1);
    }
  }

  if(world_rank == 0) {
    if(argc < 3) {
      showHelp();
      exit(1);
    } else if(argc > 4) {
      if(strcmp(argv[4], "csv") == 0) {
        printf("%i, %i", world_size, getSampleSizeEach());
      }
    }
    readAndDistribute(argv[1], world_size);
    int *bucketBoundaries = sortSample(world_size);
    distributeBucketInformation(bucketBoundaries, world_size);
    gatherResults();
    int *results;
    int resultSize = fetchResults(&results, world_size);
    writeResult(argv[2], results, resultSize);
  } else {
    int *localData;
    int dataSize = collectInputData(&localData);
    int *sample = selectSample(localData, dataSize);
    sendSample(sample);
    dataSize = distributeToCorrectBuckets(&localData, dataSize, world_size, world_rank);
    sortLocallyAndNotify(localData, dataSize, world_rank);
    sendData(localData, dataSize);
  }

  MPI_Finalize();
  return 0;
}

void showHelp() {
  printf("Please specify input and output location as commandline options\n");
}
