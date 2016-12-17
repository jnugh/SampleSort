#include "client.h"
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include "helpers.h"
#include "timer.h"
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
    *bucket = realloc(*bucket, *bucketDataSize * sizeof(int));
  }

  (*bucket)[*bucketDataCount] = value;
  (*bucketDataCount)++;
}

int getBucketIndex(int *bucketBoundaries, int world_size, int *localData, int i) {
  int value = localData[i];
  int j = 0;
  for(; j < world_size - 2; j++) {
    if(bucketBoundaries[j] > value) {
      return j;
    }
  }
  return j;
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
  MPI_Send(&value, 1, MPI_INT, j + 1, MPI_TAG_DATA, MPI_COMM_WORLD);
  return 1;
}

void receive(int **bucket, int *bucketDataCount, size_t *bucketDataSize, int sender) {
  int count;
  MPI_Recv(&count, 1, MPI_INT, sender, MPI_TAG_DATALEN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  if(*bucketDataSize <= *bucketDataCount + count) {
    *bucketDataSize = *bucketDataSize + count * 2;
    *bucket = realloc(*bucket, *bucketDataSize * sizeof(int));
  }

  MPI_Recv(&((*bucket)[*bucketDataCount]), count, MPI_INT, sender, MPI_TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  *bucketDataCount = *bucketDataCount + count;
}

int distributeToCorrectBuckets(int **localData, int dataSize, int world_size, int world_rank) {
  int bucketBoundaries[world_size - 1];
  int currentSender = 0;

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&bucketBoundaries, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&currentSender, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int *bucketBuffers[world_size - 1];
  size_t bucketBufferSizes[world_size - 1];
  int bucketBufferDataCount[world_size - 1];

  for(int i = 0; i < world_size - 1; i++) {
    bucketBufferSizes[i] = dataSize / (world_size - 1) * 2;
    bucketBufferDataCount[i] = 0;
    bucketBuffers[i] = malloc(bucketBufferSizes[i] * sizeof(int));
  }

  for(int i = 0; i < dataSize; i++) {
    int idx = getBucketIndex(bucketBoundaries, world_size, *localData, i);
    addToBucket((*localData)[i], &(bucketBuffers[idx]), &(bucketBufferDataCount[idx]), &(bucketBufferSizes[idx]));
  }

  do {
    if(currentSender == world_rank) {
      for(int i = 0; i < world_size - 1; i++) {
        if(i != world_rank - 1) {
          MPI_Send(&(bucketBufferDataCount[i]), 1, MPI_INT, i + 1, MPI_TAG_DATALEN, MPI_COMM_WORLD);
          MPI_Send(bucketBuffers[i], bucketBufferDataCount[i], MPI_INT, i + 1, MPI_TAG_DATA, MPI_COMM_WORLD);
        }
      }
    } else {
      receive(&(bucketBuffers[world_rank - 1]), &(bucketBufferDataCount[world_rank - 1]), &(bucketBufferSizes[world_rank - 1]), currentSender);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&currentSender, 1, MPI_INT, 0, MPI_COMM_WORLD);
  } while(currentSender >= 0);


  free(*localData);
  *localData = bucketBuffers[world_rank - 1];
  for(int i = 0; i < world_size - 1; i++) {
    if(i != world_rank - 1)
    free(bucketBuffers[i]);
  }
  return bucketBufferDataCount[world_rank - 1];
}

void sortLocallyAndNotify(int *data, int dataLen, int world_rank) {
  char buf[15];
  sprintf(buf, "SortLocal %d", world_rank);
  startTimer();
  sort(data, dataLen);
  stopTimer(buf);
  MPI_Barrier(MPI_COMM_WORLD);
}


void sort(int *data, int dataLen){
  qsort(data, dataLen, sizeof(int), cmpfunc);
}

void sendData(int *data, int dataLen) {
  MPI_Send(&dataLen, 1, MPI_INT, 0, MPI_TAG_RETURN_SIZE, MPI_COMM_WORLD);
  MPI_Send(data, dataLen, MPI_INT, 0, MPI_TAG_RETURN, MPI_COMM_WORLD);
}
