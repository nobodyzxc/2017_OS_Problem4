#include <bits/stdc++.h>
#include <pthread.h>
#include <termios.h>
#include "bank.h"
#include "request.h"
#include "ui.h"

#define CLIENT 25
using namespace std;

int main(void){
    srand(time(NULL));
    system("clear");
    printf("\e[?25l");
    progress(-1 , -1 , -1);
    Bank bank(1000);
    bank.active();
    RequestGenerator reqGen(bank);
    int client = CLIENT;
    bank.setLimitPayment(CLIENT);
    reqGen.active(CLIENT); //auto generate requests

    // set noecho input
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    while(1){
        //key board input
        int quota;
        scanf("%d" , &quota);
        client++;
        bank.setLimitPayment(client);
        reqGen.genReq(quota);
    }
    return 0;
}
