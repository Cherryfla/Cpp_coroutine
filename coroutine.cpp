#include "coroutine.h"


coroutine::coroutine(schedule *S, coroutine_func func, void *ud)
{
   // coroutine *co = new coroutine;
    this->func = func;
    this->ud = ud;
    this->sch = S;
    this->cap = 0;
    this->size = 0;
    this->status = COROUTINE_READY;
    this->stack = nullptr;
}

coroutine::~coroutine()
{
    if(this->stack != nullptr)
	    free(this->stack);
}

void schedule::_save_stack(coroutine *C, char *top)
{
    char dummy = 0; //to get the address of the top of the stack
    assert(top - &dummy <= STACK_SIZE);
    if(C->cap < top - &dummy){
        free(C->stack);
        C->cap = top - &dummy;
        C->stack = (char *)malloc(C->cap);
    }
    C->size = top - &dummy;
    //printf("in save_stack stack size:%d\n",C->size);
    memcpy(C->stack, &dummy, C->size);
}

schedule::schedule()
{
    //schedule *S = (schedule *)malloc(sizeof(schedule));
    // memset(S, 0, sizeof(schedule));
    //schedule *S = new schedule();
    this->nco = 0;
    this->cap = schedule::DEFAULT_COROUTINE;
    this->running = -1;
    this->co = new coroutine *[this->cap];
    for(int i = 0; i < this->cap; i++)
    {
        this->co[i] = nullptr;
    }
   // memset(S->co, 0, sizeof(coroutine *) * S->cap);
}

schedule::~schedule()
{
    for(int i=0; i < this->cap; i++)
    {
        coroutine *co = this->co[i];
        if(co != nullptr)
            delete co;
    }
    // free(S->co);
	// S->co = nullptr;
    // free(S;
    delete [] this->co;
}

int schedule::coroutine_new(coroutine_func func, void *ud)
{
    coroutine *co = new coroutine(this, func, ud);
    if(this->nco >= this->cap)  /*实际上此处只允许相等的情况*/
    {
        int id = this->cap;
        this->co = (coroutine **)realloc(this->co, this->cap * 2 * sizeof(coroutine*));//函数自带拷贝，可用vector
        memset(this->co + this->cap, 0, sizeof(coroutine *) * this->cap);
        this->co[this->cap] = co;
        this->cap *= 2;
        ++this->nco;
        return id;
    }
    else
    {
        for(int i = 0; i < this->cap; i++)
        {
            if(this->co[i] == nullptr)
            {
                this->co[i] = co;
                ++this->nco;
                return i;
            }
        }
    }
    return -1;
}

void schedule::mainfunc()
{
    int id = this->running;
    coroutine *C = this->co[id];
    C->func(this, C->ud);
/*  delete after schedule */
    delete C;
    this->co[id] = nullptr;
    --this->nco;
    this->running = -1;
}

void schedule::coroutine_resume(int id)
{
    assert(this->running == -1);
    assert(id >= 0 && id < this->cap);
    coroutine *C = this->co[id];
    this->running = id;
    if(C == nullptr)
    {
        return;
    }
    int status = C->status;
    switch(status){
        case COROUTINE_READY:
        {
            getcontext(&C->ctx);
            C->ctx.uc_stack.ss_sp = this->stack;
            C->ctx.uc_stack.ss_size = STACK_SIZE;
            C->ctx.uc_link = &this->main;
            C->status = COROUTINE_RUNNING;
           // std::function<void()> func = std::bind(&schedule::mainfunc,this);
            context_func func = reinterpret_cast<context_func>(&schedule::mainfunc);
            makecontext(&C->ctx, func, 1, this);
            swapcontext(&this->main, &C->ctx);
            break;
        }
        case COROUTINE_SUSPEND:
        {
            memcpy(this->stack + STACK_SIZE - C->size, C->stack, C->size);
            C->status = COROUTINE_RUNNING;
            swapcontext(&this->main, &C->ctx);
            break;
        }
        default:
            printf("bad status: %d\n",status);
            assert(0);
    }
}


void schedule::coroutine_yield()
{
    int id = this->running;
    assert(id >= 0);
    coroutine *C = this->co[id];
    assert((char *)&C > this->stack);
    this->_save_stack(C, this->stack + STACK_SIZE);
    C->status = COROUTINE_SUSPEND;
    this->running = -1;
    swapcontext(&C->ctx, &this->main);
}

CoroutineState schedule::coroutine_status(int id)
{
    assert(id >= 0 && id < this->cap);
    if(this->co[id] == nullptr)
    {
        return COROUTINE_DEAD;
    }
    return this->co[id]->status;
}

int schedule::coroutine_running()
{
    return this->running;
}