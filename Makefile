THREADHEADERS = start_thread.h  my_mutex.h scheduler.h
THREADFILES = start_thread.c main.c  my_mutex.c scheduler.c

EXE = main

all: 
	gcc $(THREADHEADERS) $(THREADFILES) -o $(EXE)

run_default_main:
	./$(EXE) 

clean:
	rm $(EXE)

