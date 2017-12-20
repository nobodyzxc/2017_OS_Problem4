#ifndef _BANK_H_
#define _BANK_H_
#include <vector>
#include <pthread.h>

using namespace std;

class Request;

class Bank{
    public:
        int count;
        /* count the clients finished */
        Bank(int k ,
                /* initial krona */
                void (*_display)(int , int , int) ,
                /* UI function pointer to display bank status */
                void (*_stopUI)(int));
                /* stop the whole UI or hide the bank status */

        void setLimitPayment(int count);
        /* set total clients generated */
        void active();
        /* start the thread to provide service */
        void close();
        /* stop the service (call  stopUI) */
        void reqKrona(Request *req , int amount);
        /* you can request krona from bank through the function */
        void getPayment(Request* req, int amount);
        /* give the payment to the bank */
        bool reqCheck(pair<Request*, int> newRequest);
        /* check if request can be accepted */
    private:
        int krona;
        /* the current krona bank has */
        int initKrona;
        /* the initial krona bank has */
        int limitPayments;
        /* the number of payments bank should receive */
        void (*display)(int , int , int);
        /* show bank status */
        void (*stopUI)(int);
        /* stop UI or hide bank status */

        pthread_t threadID;
        /* the bank thread */
        pthread_mutex_t queLock;
        /* mutex to protect client's request queue */
        vector<pair<Request* , int> > reqQueue;
        /* client's request queue */
        vector<Request*> personList;
        /* clients who borrow krona from bank */

        static void *running(void *ptr);
        /* thread make it run */
};
#endif
