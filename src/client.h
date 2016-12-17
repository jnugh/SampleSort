#pragma once

int collectInputData();
int *selectSample();
void sendSample(int *sample);

void collectBucketInformation();
void distributeData();

int distributeToCorrectBuckets(int **localData, int dataSize, int world_size, int world_rank);

void sortLocallyAndNotify(int *data, int dataLen, int world_rank);
void sort(int *data, int dataLen);

void sendData(int *data, int dataLen);
