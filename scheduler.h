#ifndef _scheduler_h
#define _scheduler_h
#include <time.h>
#include <string.h>

void my_thread_chsched(char *sched);

void run_threads();

void scheduler();

void sched_sorteo();

void sched_real();

void timer_interrupt();

#endif
