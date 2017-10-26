#include<pthread.h>
#include<vector>
#include<unistd.h>
#include"bank.h"
#include"request.h"
#include"ui.h"
#include<iostream>
using namespace std;


Bank::Bank(int k){
    krona = k;
    pthread_mutex_init(&queLock , NULL);
}

void Bank::active(){
    pthread_create(
            &threadID , NULL ,
            &Bank::running , this);
}

void Bank::reqKrona(Request *req , int amount){
    // who? todo:
    // use mutex to protect bank's queue
    pthread_mutex_lock(&queLock);
    reqQueue.push_back(make_pair(req , amount));
    pthread_mutex_unlock(&queLock);
}

void Bank::getPayment(int amount){
    static int count = 0;
    count++;
    krona += amount;
    progress(-1 , krona , 1000);
    if(count == 25){
        sleep(1);
        UIExit(0);
    }
    // protect krona?
    //printf("bank getpay %d\n" , amount);
}

void *Bank::running(void *ptr){
    Bank *self = (Bank *) ptr;
    vector<pair<Request* , int> > &queue = self->reqQueue;
    progress(-1 , self->krona , 1000);
    while(1){
        pthread_mutex_lock(&(self->queLock));
        if((queue).size()){
            if((queue)[0].second < self->krona){
                ((queue)[0].first)->addKrona(
                    (queue)[0].second);
                self->krona -= (queue)[0].second;
                (queue).erase((queue).begin());
                progress(-1 , self->krona , 1000);
            }
        }
        pthread_mutex_unlock(&(self->queLock));
        if(self->krona < 100){
            progress(-1 , self->krona , 1000);
            UIExit(0);
        }
    }

    // like this pointer
    // meijin todo:
    // always check the queue
    // handle requests
    // Request has quota and krona member attr
    // protect krona?
}
