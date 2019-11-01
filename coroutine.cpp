#include "coroutine.h"


coroutine::coroutine(schedule *S, coroutine_func func, void *ud)
{
   // coroutine *co = new coroutine;
    this->func = func;
    this->ud = ud;
    this->sch = S;
    this->size = 0;
    this->status = COROUTINE_READY;
    this->stack = nullptr;
}

coroutine::coroutine(const coroutine &c)
{
    this->func = c.func;
    this->ud = c.ud;
    this->ctx = c.ctx;
    this->sch = c.sch;
    this->cap = c.cap;
    this->size = c.size;
    this->status = c.status;
    this->stack = c.stack;
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
    this->running = -1;
    this->co.reserve(schedule::DEFAULT_COROUTINE);
}

schedule::~schedule()
{
    for(size_t i=0; i < this->co.size(); i++)
    {
        coroutine *co = this->co[i];
        if(co != nullptr)
            delete co;
    }
    co.clear();
}

int schedule::coroutine_new(coroutine_func func, void *ud)
{
    for(size_t i = 0; i < this->co.size(); i++)
    {
        if(this->co[i] == nullptr)
        {
            ++this->nco;
            this->co[i] = new coroutine(this, func, ud);
            return i;
        }
    }
    ++this->nco;
    this->co.emplace_back(new coroutine(this, func, ud));
    return this->co.size()-1;
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
    assert(id >= 0 && id < (int)this->co.size());
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
    assert(id >= 0 && id < (int)this->co.size());
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

int schedule::coroutine_size()
{
    return this->nco;
}