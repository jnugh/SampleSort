#pragma once

int collectInputData();
int selectSample();
void sendSample();

void collectBucketInformation();
void distributeData();

int distributeToCorrectBuckets(int **localData, int dataSize, int world_size, int world_rank);

void sort();
