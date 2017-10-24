#include <bits/stdc++.h>
#include <pthread.h>
#include "bank.h"
#include "request.h"

using namespace std;

int main(void){
    Bank bank(1000);
    bank.active();
    RequestGenerator reqGen(bank);
    reqGen.active(); //auto generate requests
    while(1){
        //key board input
        int quota;
        scanf("%d" , &quota);
        reqGen.genReq(quota);
    }
    return 0;
}
