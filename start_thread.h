#ifndef start_thread_h
#define start_thread_h

#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUMTHREADS 100
#define STACKSIZE 4096
#define INTERVAL 100

sigset_t set;
ucontext_t signal_context;

ucontext_t threads[NUMTHREADS];

int priority[NUMTHREADS];
int priority_aux[NUMTHREADS];
int tickets[NUMTHREADS];
int deadThreads[NUMTHREADS];
int curcontext;

ucontext_t *current_thread;
ucontext_t exitContext;

int init;
int active_threads;
int active_threads_aux;
int total_tickets;
int active_sched;

void my_thread_create(void *function,void *args, int tickets_sched, int priority_sched);

void my_thread_end();

void my_thread_yield();

void run_threads();

//static void setExitContext();
void *signal_stack;

static void setExitContext();

void setup();

#endif
