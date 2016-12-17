#include "quicksort.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "helpers.h"

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

void showHelp() {
  printf("Please specify input and output location as commandline options\n");
}
