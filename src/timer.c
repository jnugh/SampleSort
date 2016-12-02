#include "timer.h"
#include <time.h>
#include <stdio.h>

clock_t GLOBAL_TIMER;

void startTimer() {
  GLOBAL_TIMER = clock();
}

void stopTimer(char *title) {
  double total = (clock() - GLOBAL_TIMER) / (double)CLOCKS_PER_SEC;
  printf("Timer '%s': %f\n", title, total);
}
