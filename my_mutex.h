#ifndef _my_mutex_h
#define _my_mutex_h

#include <stdio.h>
#include <unistd.h>

#define my_mutex int

void my_mutex_init(my_mutex *lock);

void my_mutex_lock(my_mutex *lock);

void my_mutex_trylock(my_mutex *lock);

void my_mutex_unlock(my_mutex *lock);

void my_mutex_destroy(my_mutex *lock);


#endif
