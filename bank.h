#ifndef _BANK_H_
#define _BANK_H_
#include<pthread.h>
#include<vector>
#include<unistd.h>
#include"ui.h"
#include"iostream"
//#include"request.h"
using namespace std;

class Request;

class Bank{
    public:
        Bank(int k);

        void active();

        void reqKrona(Request *req , int amount);

        void getPayment(int amount);

    private:
        int krona;
        pthread_t threadID;
        pthread_mutex_t queLock;
        pthread_mutex_t krnLock;
        vector<pair<Request* , int> > reqQueue;

        static void *running(void *ptr);
};
#endif
