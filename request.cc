#include <thread>
#include <chrono>

#include <unistd.h>

#include "ui.h"
#include "bank.h"
#include "request.h"

Request::Request(Bank &bnk , int quo , int id) : bank(bnk){
    quota = quo , krona = 0 , idx = id , nextAdvance = true;
}

void Request::active(){
    pthread_create(
            &threadID , NULL ,
            &Request::running , this);
}

void Request::addKrona(int amount){
    //nextAdvance is the flag
    //to check whether req has returned
    krona += amount;
    progress(idx , krona , quota);
    if(krona == quota)
        repay();
    nextAdvance = true;
}

void Request::repay(){
    bank.getPayment(quota);
}

void Request::advanceKrona(int amount){
    if(nextAdvance){
        nextAdvance = false;
        bank.reqKrona(this , amount);
    }
}

void *Request::running(void *ptr){
    Request *self = (Request *) ptr;
    // ^ like this pointer

    progress(self->idx , self->krona , self->quota);
    while(1){
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        std::this_thread::sleep_for(std::chrono::milliseconds((rand() % 100) * 3));
#else
        usleep((rand() % 100000) * 3); //request per 1 - 3 secs
#endif
        if(self->krona < self->quota){
            int purchase =
                min(rand() % (self->quota / MIN_QUOTA) + 1 ,
                        self->quota - self->krona);
            self->advanceKrona(purchase);
        }
        else break;
    }
    progress(self->idx , self->quota + 1 , self->quota);
    return ptr;
}


RequestGenerator::RequestGenerator(Bank &bnk) : bank(bnk) {}

void RequestGenerator::active(int maximum){
    maxCust = maximum;
    curIdx = 1;
    pthread_create(
            &threadID , NULL ,
            &RequestGenerator::running , this);
}

void *RequestGenerator::running(void *ptr){
    RequestGenerator *self = (RequestGenerator *)ptr;
    //^ like this pointer

    // jeff todo:
    // poisson dist of transections here
    // use advanceKrona and repay to generate transections

    srand(time(NULL));

    //while(1){
    for(int i = 0 ;// v if maxCust == 0 , generate forever
            (!self->maxCust) || i < self->maxCust ; i++){

        self->genReq((rand() % INT_QUOTA) + MIN_QUOTA);
                        // rand quota at most 79
        //sleep(3);
        // ^ should be poisson dist
        // i.e. usleep(poissonDistTime());
    }
    return ptr;
}

void RequestGenerator::genReq(int quo){
    Request *req = new Request(bank , quo , curIdx);
    req->active();
    curIdx += 1;
}
