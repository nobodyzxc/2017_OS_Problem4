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

#define ENABLE

using namespace std;

vector<int> occ;
map<int , int> spaceidx;

struct termios ot;
struct winsize term;
pthread_mutex_t mvpLock , progLock , bookLock;
int ROWS = -4 , COLS = 0;

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

    static int idx = 0;
    while(idx < ROWS && occ[idx]) idx++;

    if(idx >= ROWS) idx = 0;
    else return idx;

    while(idx < ROWS && occ[idx]) idx++;

    if(idx >= ROWS && !(idx = 0)) return -1;
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

    for(int i = 0 ; i < 100 ; i++)
        sprintf(s , "%s%c" , s ,
                i < (cur / quo) * 100 ? MONEY : '.');

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
    mvprints(ROWS + 1 , 0 , ">");
    mvprints(ROWS + 1 , 1 , "\033[5m" CURSOR "\033[0m");

    // init mutex
    pthread_mutex_init(&mvpLock , NULL);
    pthread_mutex_init(&progLock , NULL);
    //pthread_mutex_init(&bookLock , NULL);
}

void tui_log(const char *s){
    mvprints(ROWS + 2 , 0 , "\x1b[0K");
    mvprints(ROWS + 2 , 0 , s);
}

char *tui_input(char *s){

    const int iy = ROWS + 1 , ix = 1;
    static string buffer;
    while(1) {
        int c = getchar();

        if(c == 4 || c == 3)
            tui_exit(1);
        if(c == 27){
            getchar();
            sprintf(s , "%c" , "^v><"[getchar() - 'A']);
            return s;
        }
        else if((c == '\b' || c == 127) && buffer.size())
            buffer.pop_back();
        else if(c == 21)
            buffer.clear();
        else
            buffer.push_back(c);
        mvprints(iy , ix , "\x1b[0K");
        if(c == '\n' || c == '\r'){
            mvprints(iy , ix , "\033[5m" CURSOR "\033[0m");
            break;
        }

        sprintf(s , "%s\033[5m" CURSOR "\033[0m" , buffer.c_str());
        mvprints(iy , ix , s);
    }
    mvprints(iy + 1 , 0 , "\x1b[0K"); // clear log
    sprintf(s , "%s" , buffer.c_str());
    buffer.clear();
    return s;
}

void tui_exit(int v){

    tcsetattr(STDIN_FILENO, TCSANOW, &ot);
    mvprints(ROWS + 1 , 0 , "\x1b[0K"); // clear prompt
    printf("\033[100B\033[1A");
    printf("The BANK %s\n" , v ? "went bankrupt ..." : "survived !");
    printf("\e[?25h");

    exit(v);
    return;
}
