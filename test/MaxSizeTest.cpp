#include <bits/stdc++.h>
#include "coroutine.h"

using namespace std;

void foo(schedule *S, void *ud)
{
    S->coroutine_yield();
    while(1);
}
int main(){
    int nCo = 0;
    schedule *S = new schedule();
    printf("main start\n");
    while(1)
    {
        int tempNumber = S->coroutine_new(foo, nullptr);
        S->coroutine_resume(tempNumber);
        printf("Created %dth coroutine.\n",++nCo);
    }
    printf("finished.\n");
    delete S;
    return 0;
}