#include <map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "tui.h"

#define MONEY '$' //'#'
#define EMPTY ' ' //'-'
#define CURSOR "█"

int ROWS = -7 , COLS = 0;
#define LOGROW (ROWS + 5)
#define PMTROW (ROWS + 4)
#define SHWROW (ROWS + 2)
#define PMTCOL (0)

#define ENABLE
#define EQS(a , b) (!strcmp(a , b))
using namespace std;

vector<int> occ;
map<int , int> spaceidx;

struct termios ot;
struct winsize term;
pthread_mutex_t mvpLock , progLock , bookLock;

void mvprints(int y , int x , const char *s){
#ifndef ENABLE
    return;
#endif
    pthread_mutex_lock(&mvpLock);
    printf("\033[200D\033[100A");
    if(x) printf("\033[%dC" , x);
    if(y) printf("\033[%dB" , y);
    printf(s);
    pthread_mutex_unlock(&mvpLock);
}

int book(){

    static int idx = -1;
    idx = (idx + 1) % ROWS;

    while(idx < ROWS && occ[idx]) idx++;

    if(idx >= ROWS) idx = -1;
    else return idx;

    while(idx < ROWS && occ[idx]) idx++;

    if(idx >= ROWS && (idx = -1)) return -1;
    else return idx;
}

void progress(int idx , float cur , float quo){

    if(idx < 0 && cur < 0 && quo < 0){
        for(int i = 0 ; i < ROWS ; i++)
            occ.push_back(0); // i
        return;
    }

    pthread_mutex_lock(&progLock);

    int row;

    if(spaceidx.count(idx)){
        row = spaceidx[idx];
    }
    else if(idx > 0){
        int loc = book();
        if(loc < 0){
            pthread_mutex_unlock(&progLock);
            return;
        }
        //printf("%d -> %d\n" , idx , loc);
        occ[loc] = true;
        spaceidx[idx] = loc;
        row = spaceidx[idx];
    }
    else{
        row = idx;
    }

    if(row >= 0) row++;

    char s[201] = "";

    for(int i = 0 ; i < min(COLS - 26 , 100) ; i++)
        sprintf(s , "%s%c" , s ,
                i < (cur / quo) * min(COLS - 26 , 100) ? MONEY : '.');

    if(row > 0 && cur > quo){
        for(int i = 0 ; i < (int)strlen(s) ; i++)
            if(s[i] == '$') s[i] = EMPTY;
        occ[spaceidx[idx]] = 0;
    }

    sprintf(s , "%s| <- %4d(%4d/%4d) %s\n" ,
            s , idx ,
            (int)min(cur , quo) , (int)quo ,
            row > 0 ? (cur >= quo ? "done!" : "     ") : "bank");
    mvprints(row , 0 , s);
    pthread_mutex_unlock(&progLock);
}

void tui_init(){

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &term);
    ROWS += term.ws_row;
    COLS += term.ws_col;

    // trap ctrl-c
    signal(SIGINT , tui_exit);

    //system("clear");
    printf("\033[2J");
    // set hide cursor
    printf("\e[?25l");

    // init progress
    progress(-1 , -1 , -1);

    // fprintf(stderr , "%d %d\n" , term.ws_row , term.ws_col);

    // set noecho input
    struct termios t;
    tcgetattr(STDIN_FILENO, &ot);
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    t.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    // set prompt pos
    mvprints(PMTROW , 0 , "[0]>""\033[5m" CURSOR "\033[0m");

    // init mutex
    pthread_mutex_init(&mvpLock , NULL);
    pthread_mutex_init(&progLock , NULL);
    //pthread_mutex_init(&bookLock , NULL);
}

void tui_log(const char *s){
    mvprints(LOGROW , 0 , "\x1b[0K");
    mvprints(LOGROW , 0 , s);
}

string initHis[] = {""};

string &strip(string &s){
    while(s.size() && strchr("\r\n " , s.back()))
        s.pop_back();
    return s;
}

void tui_clear(){
    for(int i = 0 ; i < (int)occ.size() ; i++){
        if(!occ[i]) mvprints(i + 1 , 0 , "\x1b[0K");
    }
}

char *tui_input(char *s){

    static string buffer;
    static int curIdx = 0;
    static vector<string> histo(initHis , initHis + 1);
    while(1) {
        int c = getchar();
        if(c == 4 || c == 3)
            tui_exit(2);
        if(c == 27){
            getchar();
            sprintf(s , "%c" , "^v><"[getchar() - 'A']);
            if(strchr("^v" , s[0])){
                histo[curIdx] = buffer;
                int nidx = curIdx + (s[0] == '^' ? -1 : 1);
                if(nidx >= 0 && nidx < (int)histo.size())
                    curIdx = nidx , buffer = histo[curIdx] , strip(buffer);
                //continue;
            }
            else{
                return s;
            }
        }
        else if(c == 16 || c == 14){ //ctrl-p , ctrl-n
            histo[curIdx] = buffer;
            int nidx = curIdx + 15 - c;
            if(nidx >= 0 && nidx < (int)histo.size())
                curIdx = nidx , buffer = histo[curIdx] , strip(buffer);
        }
        else if((c == '\b' || c == 127) && buffer.size())
            buffer.pop_back();
        else if(c == 21)
            buffer.clear();
        else
            buffer.push_back(c);
        mvprints(PMTROW , PMTCOL , "\x1b[0K");
        if((c == '\n' || c == '\r') && strip(buffer).size()){
            break;
        }

        sprintf(s , "[%d]>%s\033[5m" CURSOR "\033[0m" ,
                (int)histo.size() - 1 , buffer.c_str());
        mvprints(PMTROW , PMTCOL , s);
    }
    mvprints(LOGROW , 0 , "\x1b[0K");
    curIdx = histo.size();
    sprintf(s , "[%d]>\033[5m" CURSOR "\033[0m" , curIdx);
    mvprints(PMTROW , PMTCOL , s);
    histo.back() = buffer;
    histo.push_back("");
    sprintf(s , "%s" , buffer.c_str());
    buffer.clear();

    if(EQS(s , "clear")){
        tui_clear();
        return tui_input(s);
    }
    return s;
}

void tui_exit(int v){

    char msg[][30] = {"survived !" , "went bankrupt ..." , "stopped"};
    tcsetattr(STDIN_FILENO, TCSANOW, &ot);
    mvprints(PMTROW , 0 , "\x1b[0K"); // clear prompt
    mvprints(LOGROW , 0 , "\x1b[0K"); // clear console.log
    printf("\033[100B\033[1A");
    printf("The BANK %s\n" , msg[v]);
    printf("\e[?25h");

    exit(v);
    return;
}
