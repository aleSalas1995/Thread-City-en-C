#include "start_thread.h"
#include "scheduler.h"

//cambia el scheduling actual
void my_thread_chsched(char *sched){
    if(!strcmp(sched,"RoundRobin")){
        active_sched = 0;
    }
    if(!strcmp(sched,"Sorteo")){
        active_sched = 1;
    }
    if(!strcmp(sched,"Real")){
        active_sched = 2;
    }
}

/* Scheduling Round Robin */
void scheduler(){
    if(active_threads_aux > 0){
        curcontext = (curcontext + 1);
        if(deadThreads[curcontext% active_threads]){
            while(deadThreads[curcontext% active_threads]){
                curcontext+=1;
            }
        }
        curcontext = curcontext % active_threads;
        current_thread = &threads[curcontext];
        setcontext(current_thread);//activa el nuevo hilo
    }
}

//scheduling de sorteo
void sched_sorteo(){
    srand(time(NULL));
    int aux;
    if(active_threads_aux > 0){
        int winner = rand()%(total_tickets+1);//saca el ganador del sorteo
        aux = winner;
        for (int i = 0; i < active_threads; i++) {//revisa quien es el ganador
            aux -= tickets[i];
            if(aux<=0){
                if(deadThreads[i% active_threads]){
                    while(deadThreads[i% active_threads]){
                        i+=1;
                    }
                }
                curcontext = i;
                current_thread = &threads[curcontext];
                break;
            }else{
                tickets[i]++;
                total_tickets++;
            }
        }
        setcontext(current_thread);//activa el nuevo hilo
    }
}

//scheduling de tiempo real
void sched_real(){
    int aux = -1;
    //int last = curcontext;
    if(active_threads_aux > 0){
        for (int i = 0; i < active_threads; i++) {//busca hilo de mayor prioridad que no haya terminado
            if(aux<priority[i]&&!deadThreads[i] && !priority_aux[i]){
                curcontext = i;
                aux = priority[i];
            }
        }
        if(aux == -1){
            for (int i = 0; i < active_threads; i++) {
                priority_aux[i] = 0;
            }
            sched_real();
        }else{
            priority_aux[curcontext] = 1;//pone el hilo como ya ejecutado para que no se encicle
            current_thread = &threads[curcontext];//activa el nuevo hilo
            setcontext(current_thread);
        }
    }
}

void timer_interrupt(){
    getcontext(&signal_context);
    signal_context.uc_stack.ss_sp = signal_stack;
    signal_context.uc_stack.ss_size = STACKSIZE;
    signal_context.uc_stack.ss_flags = 0;
    sigemptyset(&signal_context.uc_sigmask);
    if(active_sched == 0){makecontext(&signal_context, scheduler, 1);}
    if(active_sched == 1){makecontext(&signal_context, sched_sorteo, 1);}
    if(active_sched == 2){makecontext(&signal_context, sched_real, 1);}
    swapcontext(current_thread,&signal_context);
}
