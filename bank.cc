#include <iostream>
#include <algorithm>
#include <cstdio>
#include "bank.h"
#include "request.h"

using namespace std;

Bank::Bank(
        int k ,
        void (*_display)(int , int , int) ,
        void (*_stopUI)(int)){
    krona = k;
    count = 0;
    initKrona = k;
    stopUI = _stopUI;
    limitPayments = 0;
    display = _display;
    pthread_mutex_init(&queLock, NULL);
}

void Bank::setLimitPayment(int count){
    limitPayments = count;
}

void Bank::active(){
    pthread_create(
            &threadID , NULL ,
            &Bank::running , this);
}

void Bank::reqKrona(Request *req , int amount){
    // who? todo:
    // use mutex to protect bank's queue
    pthread_mutex_lock(&queLock);
    reqQueue.push_back(make_pair(req , amount));
    pthread_mutex_unlock(&queLock);
}

void Bank::getPayment(Request* req, int amount){
    // protect krona?
    count++;
    //printf("krona = %d + amount + %d = %d\n" , krona , amount , krona + amount);
    krona += amount;
    for(auto clt = personList.begin() ;
            clt != personList.end() ; clt++)
        if(req -> idx == (*clt) -> idx){
            personList.erase(clt);
            break;
        }
    display(-1 , krona , initKrona);
}

void Bank::close(){
    if(count >= limitPayments)
        stopUI(0);
    else
        stopUI(1);
}

bool cmp(Request* a, Request* b){
    int aleft = a -> quota - a -> krona, bleft = b -> quota - b -> krona;
    return aleft < bleft || (aleft == bleft && a -> quota > b -> quota);
}

bool Bank :: reqCheck(pair<Request*, int> reqInfo){
    sort(personList.begin(), personList.end(), cmp);
    Request *req = reqInfo.first;
    int amount = reqInfo.second;
    int remain = krona - amount;
    //check whether the bank can accept this request or not
    if(remain >= req -> quota - (req -> krona + amount))
        return true;
    for(auto clt = personList.begin() ;
            clt != personList.end() ; clt++){

        if(remain >= req -> quota - (req -> krona + amount))
            return true;
        else if(remain < (*clt) -> quota - (*clt) -> krona)
            return false;
        remain += (*clt) -> krona;
    }
    return false;
}

void *Bank::running(void *ptr){
    Bank *self = (Bank *) ptr;
    /// ^ like this pointer
    vector<pair<Request* , int> > &queue = self -> reqQueue;
    vector<Request*>& persons = self -> personList;
    self -> display(-1 , self -> krona , self -> initKrona);

    // meijin todo:
    // always check the queue
    // handle requests
    // Request has quota and krona member attr.
    // Do we need to protect bank's krona?

    while(1){
        pthread_mutex_lock(&(self -> queLock));

        if((queue).size()){
            // your algorithm here
            // decide the priority of the queue
            // i.e. makePriority(queue);
            auto vip = queue.front();
            if(self -> reqCheck(vip)){
                bool flag = false;
                for(auto clt = persons.begin();
                        clt != persons.end() ; clt++)
                    if(vip.first -> idx == (*clt) -> idx)
                        flag = true;
                if(flag == false){
                    persons.push_back(vip.first);
                }
                self -> krona -= vip.second;
                vip.first -> addKrona(
                    vip.second);
                queue.erase(queue.begin());
                self -> display(-1 , self -> krona , self -> initKrona);
            }
            else{
                queue.push_back(vip);
                queue.erase(queue.begin());
            }
        }

        pthread_mutex_unlock(&(self -> queLock));

        if(self -> krona < 0){
            self -> display(-1 , self -> krona , self -> initKrona);
            self -> stopUI(1);
        }
    }
    return ptr;
}
