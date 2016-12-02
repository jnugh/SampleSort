#include "quicksort.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    showHelp();
    exit(1);
  }
  int *data;
  char *inPath = argv[1];
  char *outPath = argv[2];
  int dataLen = readData(inPath, &data);
  startTimer();
  qsort(data, dataLen, sizeof(int), cmpfunc);
  stopTimer("Sort");
  writeResult(outPath, data, dataLen);
  free(data);
}

int readData(char *path, int **result) {
  startTimer();
  FILE *inFile = fopen(path, "r");
  char *line;
  int lineCount = -1;
  size_t len = 0;
  ssize_t read;
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

void showHelp() {
  printf("Please specify input and output location as commandline options\n");
}
