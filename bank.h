#ifndef _BANK_H_
#define _BANK_H_
#include<pthread.h>
#include<vector>
using namespace std;

class Request;

class Bank{
    public:
        Bank(int k){ krona = k; }

        void active(){
            pthread_create(&threadID , NULL ,
                    &Bank::running , this);
        }

        void getPayment(int amount){
            krona += amount;
        }

        void reqKrona(Request *req , int amount){
            // who? todo:
            // use mutex to protect bank's queue
            reqQueue.push_back(make_pair(req , amount));
        }

    private:
        int krona;
        pthread_t threadID;
        vector<pair<Request* , int> > reqQueue;

        static void *running(void *ptr){
            Bank *self = (Bank *) ptr;
                             // like this pointer
            // meijin todo:
            // always check the queue
            // handle requests
            // Request has quota and krona member attr
        }
};
#endif
