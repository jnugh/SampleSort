#pragma once

void readAndDistribute(const char *inputFile, int world_size);
int *sortSample(int world_size);
void distributeBucketInformation(int *bucketBoundaries, int world_size);
void gatherResults();
int fetchResults(int **data, int world_size);
