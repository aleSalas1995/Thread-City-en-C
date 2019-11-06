#include "scheduler.h"
#include "start_thread.h"

//funcion que se ejecuta cuando un hilo termina
static void executeExitContext(){
    //printf("Hilo %d terminando\n", curcontext);
    deadThreads[curcontext] =1;
    total_tickets-=tickets[curcontext];
    active_threads_aux--;
    timer_interrupt(); //Selecciona el scheduler
}

//crea contexto para ejecutar cuando un hilo termina
static void setExitContext(){
    static int exitContextCreated;
    if(!exitContextCreated){
        getcontext(&exitContext);
        exitContext.uc_link = 0;
        exitContext.uc_stack.ss_sp = malloc(STACKSIZE);
        exitContext.uc_stack.ss_size = STACKSIZE;
        exitContext.uc_stack.ss_flags= 0;
        makecontext(&exitContext, (void (*) (void))&executeExitContext, 0);
        exitContextCreated = 1;
    }
}

/*inicializa el systema de hilos
 */
void setup(){
	for(int i = 0;i <NUMTHREADS ;i++){
		deadThreads[i] = 0;
	}

	setExitContext();
	
	//inicializa el struct para generar el hilo
    struct itimerval it;
    signal_stack = malloc(STACKSIZE);
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = INTERVAL * 1000;
    it.it_value = it.it_interval;
    setitimer(ITIMER_REAL, &it, NULL);//setea el timer
    struct sigaction act;
    act.sa_sigaction = timer_interrupt;//Selecciona el scheduler
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&set); 
    sigaddset(&set, SIGALRM);
    sigaction(SIGALRM, &act, NULL);

}

void my_thread_create(void *function, void *args, int tickets_sched, int priority_sched){
    if(!init){//Si no ha hecho ningun hilo, setea todo
        setup();
    }
    void *stack;
    //crea el contexto del hilo
    ucontext_t *uc = &threads[active_threads];
    getcontext(uc); //inicializa el makecontext
    stack = malloc(STACKSIZE);
    uc->uc_stack.ss_sp = stack;
    uc->uc_stack.ss_size = STACKSIZE;
    uc->uc_stack.ss_flags = 0;
    uc->uc_link = &exitContext; // se reinicia antes del makecontext()
    sigemptyset(&uc->uc_sigmask);
    //printf("%s", uc);
    makecontext(uc, function, 1, args);
    //pone datos para schedulers
    tickets[active_threads] = tickets_sched;
    priority[active_threads] = priority_sched;
    total_tickets+=tickets_sched;
    active_threads++;
    active_threads_aux++;
//setcontext(uc);
    //printf("Se creo un hilo de manera correcta");
}

//metodo que termina el hilo
void my_thread_end(){
    deadThreads[curcontext] =1;
    total_tickets-=tickets[curcontext];
    active_threads_aux--;
    timer_interrupt();//Selecciona el scheduler
}

//metodo que cesde el procesador
void my_thread_yield(){
    timer_interrupt();//Selecciona el scheduler
}

//metodo que pone a correr los hilos//
void run_threads(){
    current_thread = &threads[0];
    setcontext(&threads[0]);
}
