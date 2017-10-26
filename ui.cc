#include"ui.h"
#include<cstdlib>
#include<algorithm>
#include<cstring>
#include<termios.h>

#define MONEY '$' //'#'
#define EMPTY '-'


using namespace std;
void progress(int row , float cur , float quo){
    static pthread_mutex_t mutex;
    if(row < 0 && cur < 0 && quo < 0){
        pthread_mutex_init(&mutex , NULL);
        return;
    }
    pthread_mutex_lock(&mutex);
    //printf("\n|%d| = %f\n" , row , cur);
    printf("\033[100A\033[%dB" , row % 36);
    char s[101] = "";
    for(int i = 0 ; i < 100 ; i++)
        sprintf(s , "%s%c" , s ,
                i < (cur / quo) * 100 ? MONEY : '.');

    if(row > 0 && cur >= quo){
        for(int i = 0 ; i < (int)strlen(s) ; i++)
            if(s[i] == '$') s[i] = EMPTY;
    }

    printf("%s| <- %2d(%4d/%4d) %s\n" ,
            s , row ,
            (int)min(cur , quo) , (int)quo ,
            row > 0 ? (cur >= quo ? "done!" : "") : "bank");
    pthread_mutex_unlock(&mutex);
}

void UIExit(int v){
    printf("\033[100B\033[2A");
    printf("The BANK %s\n\n" , v ? "went bankrupt ..." : "survived !");
    printf("\e[?25h");

    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    exit(v);
    return;
}
