#ifndef _REQUEST_H_
#define _REQUEST_H_
#include <pthread.h>

#define MIN_QUOTA 20
#define INT_QUOTA 50
#define MAX_QUOTA (MIN_QUOTA + INT_QUOTA - 1)

class Bank;
class RequestGenerator;

class Request{
    public:
        bool nextAdvance;

        int quota , krona , idx;

        void (*display)(int , float , float);

        Request(Bank &bnk ,
                RequestGenerator &gen ,
                int quo , int id);

        void active();

        void addKrona(int amount);

    private:
        Bank &bank;
        pthread_t threadID;
        RequestGenerator &generator;

        static void *running(void *ptr);

        void advanceKrona(int amount);
				
        void repay();
};

class RequestGenerator{
    public:
        RequestGenerator(
                Bank &bnk ,
                void (*_display)(int , float , float));

        void active(int maximum);

        void genReq(int quo);

        void (*display)(int , float , float);

    private:
        Bank &bank;
        pthread_t threadID;
        int curIdx , maxCust;

        static void *running(void *ptr);
};
#endif
