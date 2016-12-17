#include "client.h"
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include "mpi_tags.h"

int collectInputData(int **data){
  int recLen;
  MPI_Recv(&recLen, 1, MPI_INT, 0, MPI_TAG_DATALEN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  *data = malloc(recLen * sizeof(int));
  MPI_Recv(*data, recLen, MPI_INT, 0, MPI_TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  return recLen;
}

int selectSample(int *data, int dataLen){
  return data[rand() % dataLen];
}

void sendSample(int sample){
  MPI_Send(&sample, 1, MPI_INT, 0, MPI_TAG_BOUNDARY, MPI_COMM_WORLD);
}

void addToBucket(int value, int **bucket, int *bucketDataCount, size_t *bucketDataSize) {
  if(*bucketDataSize <= *bucketDataCount) {
    *bucketDataSize = *bucketDataSize * 2;
    *bucket = realloc(*bucket, *bucketDataSize);
  }
  *bucket[*bucketDataCount] = value;
  (*bucketDataCount)++;
}

int enqueue(int *bucketBoundaries, int world_size, int world_rank, int *localData, int i) {
  int value = localData[i];
  int j = 0;
  for(; j < world_size - 2; j++) {
    if(bucketBoundaries[j] > value) {
      break;
    }
  }
  if(j + 1 == world_rank) {
    return 0;
  }
  printf("send %d to %d\n", value, j+1);
  MPI_Send(&value, 1, MPI_INT, j + 1, MPI_TAG_DATA, MPI_COMM_WORLD);
  return 1;
}

int receiveNonBlocking(int **bucket, int *bucketDataCount, size_t *bucketDataSize) {
  return 1;
}

int distributeToCorrectBuckets(int **localData, int dataSize, int world_size, int world_rank) {
  int bucketBoundaries[world_size - 1];
  int status = 0;
  int enqueued = 0;
  size_t bucketDataSize = dataSize * 2;
  int *bucketData = malloc(bucketDataSize * sizeof(int));
  int bucketDataCount = 0;

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&bucketBoundaries, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);

  while(1 == 1) {
    if(enqueued < dataSize) {
      if(!enqueue(bucketBoundaries, world_size, world_rank, *localData, enqueued)) {
        addToBucket((*localData)[enqueued], &bucketData, &bucketDataCount, &bucketDataSize);
      }
      enqueued++;
    } else if(enqueued == dataSize) {
      MPI_Send(&status, 1, MPI_INT, 0, MPI_TAG_SENDERSTATUS, MPI_COMM_WORLD);
      enqueued++;
    }
    if(receiveNonBlocking(&bucketData, &bucketDataCount, &bucketDataSize) == 1) { //Master said we are done now...
      break;
    }
  }

  free(*localData);
  *localData = bucketData;
  return bucketDataSize;
}

void distributeData(){

}

void sort(){

}
