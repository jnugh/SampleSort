#pragma once

int main(int argc, char *argv[]);

/**
 * Returns the length of result or -1 if an error occured.
 * Input file's first line needs to specify the number of lines following
 */
int readData(char *inputFile, int **result);
void writeResult(char *outPath, int *data, int dataLen);

void showHelp();
