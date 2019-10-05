#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

#if __APPLE__ && __MACH__
    #include <sys/ucontext.h>
#else
    #include <ucontext.h>
#endif

#define STACK_SIZE (1024*1024)
#define DEFAULT_COROUTINE 16


enum CoroutineState{
    COROUTINE_DEAD, COROUTINE_READY, COROUTINE_RUNNING, COROUTINE_SUSPEND
};

struct coroutine;
struct schedule;

typedef void (*coroutine_func)(schedule *, void *ud);

coroutine* _co_new(schedule *S, coroutine_func func, void *ud);
void _co_delete(coroutine *co);
struct schedule* coroutine_open(void);
void coroutine_close(schedule *S);
int coroutine_new(schedule *S, coroutine_func func, void *ud);
void coroutine_resume(schedule *S, int id);
void coroutine_yield(schedule *S);
int coroutine_status(schedule *S, int id);
int coroutine_running(schedule *S);

#endif
