#include <bits/stdc++.h>
#include "coroutine.h"

using namespace std;

void foo(schedule *S, void *ud)
{
    int *arg = (int *)ud;
    for(int i = 0; i < 5; i++)
    {
        printf("Coroutine %d : %d \n", S->coroutine_running(), *arg + i);
        S->coroutine_yield();
    }
}
int main(){
    schedule *S = new schedule();
    int arg1 = 0;
    int arg2 = 100;
    int co1 = S->coroutine_new(foo, &arg1);
    int co2 = S->coroutine_new(foo, &arg2);
    printf("main start\n");
    while(S->coroutine_status(co1) && S->coroutine_status(co2))
    {
        S->coroutine_resume(co1);
        S->coroutine_resume(co2);
    }
    printf("finished.\n");
    return 0;
}