#ifndef _BANK_H_
#define _BANK_H_
#include <vector>
#include <pthread.h>

using namespace std;

class Request;

class Bank{
    public:
        Bank(int k);

        void setLimitPayment(int count);

        void active();

        void reqKrona(Request *req , int amount);

        void getPayment(int amount);

    private:
        int krona;
        int initKrona;
        int limitPayments;

        pthread_t threadID;
        pthread_mutex_t queLock;
        pthread_mutex_t krnLock;
        vector<pair<Request* , int> > reqQueue;

        static void *running(void *ptr);
};
#endif
