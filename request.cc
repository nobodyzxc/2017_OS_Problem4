#include <thread>
#include <chrono>

#include <sys/timeb.h>
#include <unistd.h>

#include "bank.h"
#include "request.h"

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
    if(krona + amount == quota) repay();
    nextAdvance = true;
    krona += amount;
}

void Request::repay(){
    display(idx , quota + 1 , quota);
    bank.getPayment(this, quota);
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
        // jeff todo:
        // impl exp_dist func for transections here

#ifdef __GXX_EXPERIMENTAL_CXX0X__
        std::this_thread::sleep_for(
                std::chrono::milliseconds((rand() % 100) * 3));
        //milliseconds(exp_dist());
#else
        usleep((rand() % 100000) * 3); //request per 1 - 3 secs
        //usleep(exp_dist());
#endif
        if(self->krona < self->quota){
            int purchase =
                min(rand() % (self->quota / MIN_QUOTA) + 1 ,
                        self->quota - self->krona);
            self->advanceKrona(purchase);
        }
        else break;
    }

    //self->display(self->idx , self->quota + 1 , self->quota);
    // mv to repay

    // generate new client before being deleted
    struct timeb timeBuf;
    ftime(&timeBuf);
    srand(timeBuf.millitm);

#ifdef SPAWN
    int child = rand() % 7 / 3;
    while(child--)
        (self->generator).genReq(0);
#endif

    delete self;
    return ptr;
}


RequestGenerator::RequestGenerator(
        Bank &bnk ,
        void (*_display)(int , float , float)) : bank(bnk) {
    power = false;
    display = _display;
    maxCust = 0;
    curIdx = 1;
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

    for(int i = 0 ;// v if maxCust == 0 , generate forever
            (!self->maxCust) || i < self->maxCust ; i++){
        if(self->power)
            self->genReq(0);
        // rand quota at most 79
        sleep(rand() % 3 + 2);
        // ^ should be poisson dist
        // i.e. usleep(poissonDistTime());
    }
    return ptr;
}

void RequestGenerator::genReq(int quo){
    if(quo <= 0) quo = (rand() % INT_QUOTA) + MIN_QUOTA;
    (new Request(bank , *this , quo , curIdx))->active();
    bank.setLimitPayment(curIdx);
    curIdx += 1;
}
