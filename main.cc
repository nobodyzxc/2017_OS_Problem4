#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <signal.h>
#include <unistd.h>

#include "ui.h"
#include "bank.h"
#include "request.h"

#define CLIENT 25
using namespace std;

int main(void){

    signal(SIGINT , UIExit);
    // trap ctrl-c

    system("clear");
    printf("\e[?25l");
    // set cursor loc

    progress(-1 , -1 , -1);
    // init progress

    Bank bank(1000);
    bank.active();
    RequestGenerator reqGen(bank);

    int client = CLIENT;
    bank.setLimitPayment(CLIENT);
    reqGen.active(CLIENT);
    //auto generate requests

    // set noecho input
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    while(1){
        //key board input
        int quota;
        scanf("%d" , &quota);
        if(quota >= MIN_QUOTA && quota){
            client++;
            bank.setLimitPayment(client);
            reqGen.genReq(quota);
        }
    }
    return 0;
}
