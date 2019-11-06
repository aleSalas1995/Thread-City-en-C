#include "my_mutex.h"

void my_mutex_init(my_mutex *lock){
	lock = 0;
}

//xchg de ensamblador para hacer la operacion y asignacion atomica
int atomic_xchg(my_mutex * lock){
    unsigned int  tmp = 1;
    __asm__(
        "xchgl %0, %1;\n"
        : "=r"(tmp), "+m"(*lock)
        : "0"(tmp)
        :"memory");
    return tmp;
}

//test and set para hacer el lock
int test_and_set(my_mutex *lock){
    return atomic_xchg(lock);
}

void my_mutex_destroy(my_mutex *lock){
    //free(lock);
}

//metodo que hace el lock del mutex
void my_mutex_lock(my_mutex *lock){
    while (*lock){
        sleep(1);
    }
    test_and_set(lock);
}
//metodo para try-lock del mutex
void my_mutex_trylock(my_mutex *lock){
    while (*lock){
        usleep(1000);
    }
    test_and_set(lock);
}
//metodo que desbloquea el mutex
void my_mutex_unlock(my_mutex *lock){
    unsigned int  tmp = 0;
    __asm__(
        "xchgl %0, %1;\n"
        : "=r"(tmp), "+m"(*lock)
        : "0"(tmp)
        :"memory");
}
