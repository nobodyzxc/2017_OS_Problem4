#ifndef _REQUEST_H_
#define _REQUEST_H_
#include<bits/stdc++.h>
#include<pthread.h>
#include<unistd.h>
#include"bank.h"
#include"ui.h"

#define MIN_QUOTA 20
#define INT_QUOTA 50
#define MAX_QUOTA (MIN_QUOTA + INT_QUOTA - 1)

class Request{
    public:
        bool nextAdvance;

        int quota , krona , idx;

        Request(Bank &bnk , int quo , int id);

        void active();

        void addKrona(int amount);

    private:
        Bank &bank;
        pthread_t threadID;

        static void *running(void *ptr);

        void advanceKrona(int amount);

        void repay();
};

class RequestGenerator{
    public:
        RequestGenerator(Bank &bnk);

        
        void active(int maximum);

        void genReq(int quo);

    private:
        int curIdx;
        int maxCust;
        Bank &bank;
        pthread_t threadID;

        static void *running(void *ptr);
};
#endif
