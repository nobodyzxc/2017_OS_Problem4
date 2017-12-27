#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <thread>

#include "bank.h"
#include "request.h"

#include "gui.h"
/* UI lib should provide
 * void UIInit()
 * void UIExit(int)
 * void UIDisp(int , float , flaot) index, current value, max value
 * void UILog(char *)
 * char *INPUT(char *) */

#define CLIENT 10
#define EQS(a , b) (!strcmp(a , b))
using namespace std;

Bank bank(BK_KRONA , UIDisp , UIExit);
RequestGenerator reqGen(bank , UIDisp);

int nextInt(char *s){
    int rtn = 0;
    while(*s == ' ') s++;
    while(*s && !(*s >= '0' && *s <= '9')) s++;
    if(!*s) return rtn;
    while(*s >= '0' && *s <= '9') rtn = rtn * 10 + *s - '0' , s++;
    return rtn;
}

int clsdIntvl(int a , int b){
    return rand() % (abs(a - b) + 1) + min(a , b);
}

void parse(char *cmd){

    char msg[100];

    if(EQS(cmd , "exit")
            || EQS(cmd , "quit")
            || EQS(cmd , "bye"))
        bank.close();

    if(EQS(cmd , "auto on")
            || EQS(cmd , "auto")
            || EQS(cmd , "on")){
        reqGen.power = true;
        UILog("auto generate on");
        return;
    }

    if(EQS(cmd , "auto off")
            || EQS(cmd , "off")){
        reqGen.power = false;
        UILog("auto generate off");
        return;
    }

    int quota = 0 , inst = 1 , till = 0 , rndlmt = 0;
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

    if((p = strchr(cmd , ',')))
        rndlmt = nextInt(p + 1);

    if(rndlmt == 0) rndlmt = quota;

    int diff = till < quota ? -1 : 1;

    sprintf(msg , "from %d to %d repeat %d\n" ,
            quota , till , inst);
    UILog(msg);

    while(inst--)
        for(int q = quota ; q != till + diff ; q += diff)
            if(q >= MIN_QUOTA){
#ifdef __GXX_EXPERIMENTAL_CXX0X__
                std::this_thread::sleep_for(
                        std::chrono::milliseconds(100));
#else
                usleep(100000); //request per 1 - 3 secs
#endif
                int req = clsdIntvl(q , rndlmt);
                while(req < MIN_QUOTA) req = clsdIntvl(q , rndlmt);
                reqGen.genReq(req);
            }
            else{
                char msg[100];
                sprintf(msg , "req %d < %d (MIN_QUOTA)\n" , q , MIN_QUOTA);
                UILog(msg);
            }
}

int main(void){

    UIInit();
    bank.active();

    //auto generate requests (CLIENT)
    reqGen.active(20);

    //key board input
#if 0
    char str[30];
    while(1) parse(INPUT(str));
#endif
    while(1){
        string inst = c.receive(512);
        cout << inst << endl;
    }
    return 0;
}
