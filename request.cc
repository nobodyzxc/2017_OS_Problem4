#include<bits/stdc++.h>
#include<pthread.h>
#include<unistd.h>
#include"request.h"
#include"bank.h"
#include"ui.h"

Request::Request(Bank &bnk , int quo , int id) : bank(bnk){
    quota = quo , krona = 0 , idx = id , nextAdvance = true;
}

void Request::active(){
    pthread_create(
            &threadID , NULL ,
            &Request::running , this);
}

void Request::addKrona(int amount){
    // you can set a flag
    // to check previous req has returned
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
    // like this pointer
    // jeff todo:
    // poisson dist of transections here
    // use advanceKrona and repay to generate transections
    progress(self->idx , self->krona , self->quota);
    while(1){
        usleep(rand() % 3000000); //request per 1 - 3 secs
        if(self->krona < self->quota)
            self->advanceKrona((rand() % ((self->quota - self->krona))) + 1);
        else break;
    }
    progress(self->idx , self->quota + 1 , self->quota);
}


RequestGenerator::RequestGenerator(Bank &bnk) : bank(bnk) {}

void RequestGenerator::active(){
    pthread_create(
            &threadID , NULL ,
            &RequestGenerator::running , this);
}

void *RequestGenerator::running(void *ptr){
    RequestGenerator *self = (RequestGenerator *)ptr;
    //like this pointer
    // zxc todo:
    // auto gen request
    // very naive method
    srand(time(NULL));
    while(1){
        self->genReq((rand() % 20) + 20);
        //sleep(3);
    }
}

void RequestGenerator::genReq(int quo){
    static int count = 1;
    Request *req = new Request(bank , quo , count);
    req->active();
    count += 1;
    if(count > 25) pthread_exit(0);
}
