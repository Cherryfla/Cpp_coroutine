#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <functional>

#if __APPLE__ && __MACH__
    #include <sys/ucontext.h>
#else
    #include <ucontext.h>
#endif

#define STACK_SIZE (1024*1024)

enum CoroutineState{
    COROUTINE_DEAD, COROUTINE_READY, COROUTINE_RUNNING, COROUTINE_SUSPEND
};

class coroutine;
class schedule;

typedef void (*coroutine_func)(schedule *, void *ud);
typedef void (*context_func)(void);

class schedule
{
    private:
        static const int DEFAULT_COROUTINE = 16;    //硬编码不合适
        char stack[STACK_SIZE];
        ucontext_t main;
        int nco;
        int cap;
        int running;
        coroutine **co;

        void mainfunc();
        void _save_stack(coroutine *C, char *top);
    public:
        schedule();
        ~schedule();
        int coroutine_new(coroutine_func func, void *ud);
        void coroutine_resume(int id);
        void coroutine_yield();
        CoroutineState coroutine_status(int id);
        int coroutine_running();
};

class coroutine
{
    private:
        void *ud;
        ucontext_t ctx;
        struct  schedule *sch;
        ptrdiff_t cap;
        ptrdiff_t size;
        CoroutineState status;
        char *stack;
        
        friend class schedule;
    public:
        coroutine_func func;
        coroutine();
        coroutine(schedule *S, coroutine_func func, void *ud);
        //coroutine(const coroutine &c);
        ~coroutine();

};

#endif
