#include "helpers.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

int sampleSizeEach = 50;
void setSampleSizeEach(int newSampleSizeEach) {
  sampleSizeEach = newSampleSizeEach;
}

int getSampleSizeEach() {
  return sampleSizeEach;
}

int readData(const char *path, int **result) {
  startTimer();
  FILE *inFile = fopen(path, "r");
  char *line = NULL;
  int lineCount = -1;
  size_t len = 0;
  size_t read;
  if(inFile == NULL) {
    printf("Could not open input file.\n");
    exit(1);
  }

  read = getline(&line, &len, inFile);
  if(read == -1) {
    printf("File is empty\n");
    exit(1);
  }

  if(sscanf (line, "%i", &lineCount) !=1) {
    printf("Could not read line count\n\n");
    exit(1);
  }

  *result = (int*)malloc(lineCount * sizeof(int));

  for(int i = 0; i < lineCount; i++) {
    if((read = getline(&line, &len, inFile)) == -1) {
      printf("line count missmatch\n");
      exit(1);
    }

    if(sscanf (line, "%i", &((*result)[i])) !=1) {
      printf("line %i is not a number\n", i);
      exit(1);
    }
  }

  free(line);
  fclose(inFile);
  stopTimer("ReadSample");
  return lineCount;
}

void writeResult(char *outPath, int *data, int dataLen) {
  startTimer();
  FILE *outFile = fopen(outPath, "w");
  if(outFile == NULL) {
    printf("Could not open outfile\n");
    exit(1);
  }

  fprintf(outFile, "%d\n", dataLen);

  for(int i = 0; i < dataLen; i++) {
    fprintf(outFile, "%d\n", data[i]);
  }
  fclose(outFile);
  stopTimer("WriteResult");
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}
