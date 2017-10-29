#include <map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <termios.h>

#include "tui.h"

#define MONEY '$' //'#'
#define EMPTY ' ' //'-'
#define SCRHI 36

using namespace std;

vector<int> space;
map<int , int> spaceidx;

int avail(){
    static int idx = 0;
    while(idx < SCRHI && space[idx]) idx++;
    if(idx >= SCRHI) idx = 0;
    else return idx;
    while(idx < SCRHI && space[idx]) idx++;
    if(idx >= SCRHI){
        idx = 0;
        return -1;
    }
    else return idx;
}

void progress(int idx , float cur , float quo){
    int row;
    static pthread_mutex_t mutex;

    if(idx < 0 && cur < 0 && quo < 0){
        pthread_mutex_init(&mutex , NULL);
        for(int i = 0 ; i < SCRHI ; i++)
            space.push_back(0);
        return;
    }

    pthread_mutex_lock(&mutex);


    if(spaceidx.count(idx)){
        row = spaceidx[idx];
    }
    else if(idx > 0){
        int loc = avail();
        if(loc < 0) return;
        space[loc] = idx;
        spaceidx[idx] = loc;
        row = spaceidx[idx];
    }
    else{
        row = idx;
    }

    printf("\033[100A\033[%dB" , row >= 0 ? ++row : row);

    char s[101] = "";
    for(int i = 0 ; i < 100 ; i++)
        sprintf(s , "%s%c" , s ,
                i < (cur / quo) * 100 ? MONEY : '.');

    if(row > 0 && cur >= quo){
        for(int i = 0 ; i < (int)strlen(s) ; i++)
            if(s[i] == '$') s[i] = EMPTY;
        space[spaceidx[idx]] = 0;
    }

    printf("%s| <- %4d(%4d/%4d) %s\n" ,
            s , idx ,
            (int)min(cur , quo) , (int)quo ,
            row > 0 ? (cur >= quo ? "done!" : "     ") : "bank");

    pthread_mutex_unlock(&mutex);
}

void UIExit(int v){
    printf("\033[100B\033[1A");
    printf("The BANK %s\n" , v ? "went bankrupt ..." : "survived !");
    printf("\e[?25h");

    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    exit(v);
    return;
}
