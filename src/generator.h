#pragma once

#include <stdio.h>

int main(int argc, char *argv[]);
int getDesiredOutputSize(int argc, char *argv[]);
FILE *openOutputFile(int argc, char *argv[]);

void displayHelp();

void seedRng();

void writeRandomNumbers(int count, FILE *fp);
int getNumber();
