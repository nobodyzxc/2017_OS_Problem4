#ifndef _BANK_H_
#define _BANK_H_
#include <vector>
#include <pthread.h>

using namespace std;

class Request;

class Bank{
    public:
        int count;

        Bank(int k ,
                void (*_display)(int , float , float) ,
                void (*_stopUI)(int));

        void setLimitPayment(int count);

        void active();

        void close();

        void reqKrona(Request *req , int amount);

        void getPayment(Request* req, int amount);

        bool reqCheck(pair<Request*, int> newRequest);

    private:
        int krona;
        int initKrona;
        int limitPayments;
        void (*display)(int , float , float);
        void (*stopUI)(int);

        pthread_t threadID;
        pthread_mutex_t queLock;
        pthread_mutex_t krnLock;
        pthread_mutex_t pltLock;
        vector<pair<Request* , int> > reqQueue;
        vector<Request*> personList;

        static void *running(void *ptr);
};
#endif
