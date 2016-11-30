#include "generator.h"

#include <time.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char *argv[]) {
  int count = getDesiredOutputSize(argc, argv);
  FILE *out = openOutputFile(argc, argv);
  seedRng();

  writeRandomNumbers(count, out);
}

void writeRandomNumbers(int count, FILE *fp) {
  for(int i = 0; i < count; i++) {
    fprintf(fp, "%d\n", getNumber());
  }
}

int getNumber(void) {
  return rand() % INT_MAX;
}

int getDesiredOutputSize(int argc, char *argv[]) {
  if(argc > 1) {
    int count = -1;
    if (sscanf (argv[1], "%i", &count) !=1 || count <= 0 ) {
      printf ("The first argument has to be a number > 0");
      exit(1);
    }
    return count;
  } else {
    displayHelp();
    exit(1);
  }
}

FILE *openOutputFile(int argc, char *argv[]) {
  if(argc > 2) {
    FILE *fp = fopen(argv[2], "w");
    if(fp == NULL) {
      printf("Could not open output file.");
      exit(1);
    }
    return fp;
  } else {
    displayHelp();
    exit(1);
  }
}

void displayHelp() {
  printf("Please specify exaclty 2 parameteers: \n[Number of Integers] [path to output]\n");
}

void seedRng() {
  srand(time(NULL));
}
