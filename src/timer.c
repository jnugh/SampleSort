#include "timer.h"
#include <time.h>
#include <stdio.h>

clock_t GLOBAL_TIMER;
int timerMode = 0;

void setTimerMode(int mode) {
  if(mode < 2 && mode >= 0)
    timerMode = mode;
}

void startTimer() {
  GLOBAL_TIMER = clock();
}

void stopTimer(char *title) {
  double total = (clock() - GLOBAL_TIMER) / (double)CLOCKS_PER_SEC;
  if(timerMode == 0) {
    printf("Timer '%s': %f\n", title, total);
  } else if(timerMode == 1) {
    printf(", %f", total);
  }
}
