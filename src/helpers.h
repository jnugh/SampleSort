#pragma once

/**
 * Returns the length of result or -1 if an error occured.
 * Input file's first line needs to specify the number of lines following
 */
int readData(const char *inputFile, int **result);
void writeResult(char *outPath, int *data, int dataLen);

void setSampleSizeEach(int sampleSizeEach);
int getSampleSizeEach();

int cmpfunc (const void * a, const void * b);
