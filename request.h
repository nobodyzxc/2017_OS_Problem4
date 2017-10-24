#ifndef _REQUEST_H_
#define _REQUEST_H_
#include<pthread.h>
#include<cstdlib>
#include<unistd.h>

class Bank;
class Request{
    public:
        int quota , krona;
        Request(Bank &bnk , int quo) : bank(bnk){
            quota = quo , krona = 0;
        }

        void active(){
            pthread_create(
                    &threadID , NULL ,
                    &Request::running , this);
        }

    private:
        Bank &bank;
        pthread_t threadID;

        static void *running(void *ptr){
            Request *self = (Request *) ptr;
                             // like this pointer
            // jeff todo:
            // poisson dist of transections here
            // use advanceKrona and repay to generate transections
        }

        void addKrona(int amount){
            // you can set a flag
            // to check previous req has returned
            krona += amount;
        }

        void advanceKrona(int amount){
            bank.reqKrona(this , amount);
        }

        void repay(int amount){
            bank.getPayment(amount);
            delete this;
            pthread_exit(0);
        }
};

class RequestGenerator{
    public:
        RequestGenerator(Bank bnk) : bank(bnk) {}

        void active(){
            pthread_create(
                    &threadID , NULL ,
                    &RequestGenerator::running , this);
        }

        void genReq(int quo){
            Request *req = new Request(bank , quo);
            req->active();
        }

    private:
        Bank &bank;
        pthread_t threadID;

        static void *running(void *ptr){
            RequestGenerator *self = (RequestGenerator *)ptr;
                                //like this pointer
            // zxc todo:
            // auto gen request
            // very naive method
            while(1) sleep(3) , self->genReq((rand() % 100) +1);
        }
};
#endif
