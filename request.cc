#include <thread>
#include <chrono>

#include <sys/timeb.h>
#include <unistd.h>
#include <assert.h>

#include "bank.h"
#include "request.h"
#include "exponential.h"
#include "tui.h"

//#define SPAWN

Request::Request(
        Bank &bnk ,
        RequestGenerator &gen ,
        int quo , int id) : bank(bnk) , generator(gen){
    display = gen.display;
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
    display(idx , krona + amount , quota);
    krona += amount;
    if(krona == quota) repay();
    else nextAdvance = true;
}

void Request::repay(){
    bank.getPayment(this, quota);
    generator.flyAway(idx);
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

    self->display(self->idx , self->krona , self->quota);
    while(1){

#ifdef __GXX_EXPERIMENTAL_CXX0X__
        std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    (int)(min(exp_dist(self->generator.req_lambda), (float)5) * 1000)));
#else
        usleep((rand() % 100000) * 3); //request per 0 - 3 secs
#endif
        if(self->krona < self->quota){
            int purchase =
                min(rand() % (self->quota / AVG_REQNUM) + 1 ,
                        self->quota - self->krona);
            self->advanceKrona(purchase);
        }
        else break;
    }

    //self->display(self->idx , self->quota + 1 , self->quota);
    // mv to repay

    delete self;
    return ptr;
}


RequestGenerator::RequestGenerator(
        Bank &bnk ,
        void (*_display)(int , int , int)) : bank(bnk) {
    power = true;
    display = _display;
    maxCust = 0;
    curCust = 0;
    pthread_mutex_init(&baby_taker , NULL);
    cus_lambda = CUS_LAMBDA;
    req_lambda = REQ_LAMBDA;
}

void RequestGenerator::active(int maximum){
    maxCust = maximum;
    pthread_create(
            &threadID , NULL ,
            &RequestGenerator::running , this);
}

void *RequestGenerator::running(void *ptr){
    RequestGenerator *self = (RequestGenerator *)ptr;
    //^ like this pointer
    struct timeb timeBuf;
    ftime(&timeBuf);
    srand(timeBuf.millitm);
    puts("generator start");
    while(1){
        pthread_mutex_lock(&(self->baby_taker));
        if(self->curCust < self->maxCust){
            if(self->power) self->genReq(0);
            if(self->curCust >= self->maxCust) break;
            std::this_thread::sleep_for(
                        std::chrono::milliseconds(
                            (int)(exp_dist(self->cus_lambda) * 1000)));

            //sleep_possion();
        }
        else break;
        pthread_mutex_unlock(&(self->baby_taker));
    }
    pthread_mutex_unlock(&(self->baby_taker));
    puts("generator shutdown");
#if 0
    /* legacy below */
    for(int i = 0 ;// v if maxCust == 0 , generate forever
            (!self->maxCust) || i < self->maxCust ; i++){
        // rand quota at most 79
        sleep(rand() % 3 + 2);
        // ^ should be poisson dist
        // i.e. usleep(poissonDistTime());
    }
#endif
    return ptr;
}


int RequestGenerator::randIdx(){
    int idx;
    assert(curCust < maxCust);
    assert((int)childs.size() < maxCust);
    while(childs.find(idx = rand() % maxCust) != childs.end());
    curCust += 1;
    childs.insert(idx);
    printf("child %2d join (%d)\n" , idx + 1 , curCust);
    return idx + 1;
}

void RequestGenerator::flyAway(int idx){
    pthread_mutex_lock(&baby_taker);
    childs.erase(idx - 1);
    //printf("child %2d left , avail = %lu\n" ,
    //        idx , childs.size());
    if(curCust == maxCust) active(maxCust);
    curCust -= 1;
    pthread_mutex_unlock(&baby_taker);
}

void RequestGenerator::genReq(int quo){
    if(quo <= 0) quo = (rand() % INT_QUOTA) + MIN_QUOTA;
    int newIdx = randIdx();
    (new Request(bank , *this , quo , newIdx))->active();
    bank.setLimitPayment(curCust);
}
