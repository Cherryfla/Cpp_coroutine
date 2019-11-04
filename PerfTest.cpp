#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
#include "coroutine.h"

using namespace std;

int gCount = 0;

void hander(int arg)
{
    printf("finished.\n");
    printf("Switch %d times in a second.\n",gCount);
    exit(0);
}

void foo(schedule *S, void *ud)
{
    while(1)
        S->coroutine_yield();
}

int main(){
    schedule *S = new schedule();
    int co1 = S->coroutine_new(foo, nullptr);
    int co2 = S->coroutine_new(foo, nullptr);
    printf("Start\n");
    signal(SIGALRM, hander);    
    alarm(1);
    while(S->coroutine_status(co1) && S->coroutine_status(co2))
    {
        S->coroutine_resume(co1);
        ++gCount;
        S->coroutine_resume(co2);
        ++gCount;
    }
    //printf("finished.\n");
    delete S;
    return 0;
}