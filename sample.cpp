#include <bits/stdc++.h>
#include "coroutine.h"

using namespace std;

void foo(schedule *S, void *ud)
{
    int *arg = (int *)ud;
    for(int i = 0; i < 5; i++)
    {
        printf("Coroutine %d : %d \n",coroutine_running(S), *arg + i);
        coroutine_yield(S);
    }
}
int main(){
    schedule *S = coroutine_open();
    int arg1 = 0;
    int arg2 = 100;
    int co1 = coroutine_new(S, foo, &arg1);
    int co2 = coroutine_new(S, foo, &arg2);
    printf("main start\n");
    while(coroutine_status(S, co1) && coroutine_status(S, co2))
    {
        coroutine_resume(S, co1);
        coroutine_resume(S, co2);
    }
    printf("finished.\n");
    return 0;
}