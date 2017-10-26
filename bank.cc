#include "ui.h"
#include "bank.h"
#include "request.h"

using namespace std;

Bank::Bank(int k){
    krona = k;
    initKrona = k;
    limitPayments = 0;
    pthread_mutex_init(&queLock , NULL);
    pthread_mutex_init(&krnLock , NULL);
}

void Bank::setLimitPayment(int count){
    limitPayments = count;
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
    // protect krona?
    pthread_mutex_lock(&krnLock);
    count++;
    krona += amount;
    progress(-1 , krona , initKrona);
    if(limitPayments && count >= limitPayments){
        UIExit(0);
    }
    pthread_mutex_unlock(&krnLock);
}

void *Bank::running(void *ptr){
    Bank *self = (Bank *) ptr;
        // ^ like this pointer
    vector<pair<Request* , int> > &queue = self->reqQueue;

    progress(-1 , self->krona , self->initKrona);

    // meijin todo:
    // always check the queue
    // handle requests
    // Request has quota and krona member attr.
    // Do we need to protect bank's krona?

    while(1){
        pthread_mutex_lock(&(self->queLock));

        if((queue).size()){
            // your algorithm here
            // decide the priority of the queue
            // i.e. makePriority(queue);
            //pthread_mutex_lock(&(self->krnLock));
            if((queue)[0].second < self->krona){
                ((queue)[0].first)->addKrona(
                    (queue)[0].second);
                self->krona -= (queue)[0].second;
                (queue).erase((queue).begin());
                progress(-1 , self->krona , self->initKrona);
            }
            //pthread_mutex_unlock(&(self->krnLock));
        }

        pthread_mutex_unlock(&(self->queLock));

        if(self->krona < (MAX_QUOTA / MIN_QUOTA) + 5){
            progress(-1 , self->krona , self->initKrona);
            UIExit(1);
        }
    }
    return ptr;
}
