#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <thread>

#include "bank.h"
#include "request.h"


#include "tui.h"
#define UIInit tui_init
#define UIExit tui_exit
#define UIDisp progress
#define UILog  tui_log
#define INPUT tui_input
#include <sys/ioctl.h>

#define CLIENT 10
using namespace std;

Bank bank(1000 , UIDisp , UIExit);
RequestGenerator reqGen(bank , UIDisp);

int nextInt(char *s){
    int rtn = 0;
    while(*s == ' ') s++;
    while(*s >= '0' && *s <= '9') rtn = rtn * 10 + *s - '0' , s++;
    return rtn;
}

void parse(char *cmd){
    int quota = 0 , inst = 1 , till = 0;
    quota = nextInt(cmd);

    char arrow[4][4] = {"^" , "v" , ">" , "<"};
    for(unsigned int i = 0 ; i < 4 ; i++)
        if(!strcmp(arrow[i] , cmd)){
            char msg[100];
            sprintf(msg , "Arrow of %s" , arrow[i]);
            UILog(msg);
            return;
        }

    char mul[] = "xX*" , *p;
    for(unsigned int i = 0 ; i < strlen(mul) ; i++)
        if((p = strchr(cmd , mul[i])))
            inst = max(nextInt(p + 1) , inst);

    if((p = strchr(cmd , '-')))
        till = nextInt(p + 1);

    if(till == 0) till = quota;

    int diff = till < quota ? -1 : 1;

    while(inst--)
        for(int q = quota ; q != till + diff ; q += diff)
            if(q >= MIN_QUOTA){
#ifdef __GXX_EXPERIMENTAL_CXX0X__
                std::this_thread::sleep_for(
                        std::chrono::milliseconds(100));
#else
                usleep(100000); //request per 1 - 3 secs
#endif
                reqGen.genReq(q);
            }
            else{
                char msg[100];
                sprintf(msg , "input : %d , cannot < %d\n" , q , MIN_QUOTA);
                UILog(msg);
            }
}

int main(void){

    UIInit();
    bank.active();

    //auto generate requests
    //reqGen.active(CLIENT);

    //key board input
    char str[30];
    while(1) parse(INPUT(str));
    return 0;
}
