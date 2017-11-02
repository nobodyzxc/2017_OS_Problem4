#include <algorithm>
#include <cstdio>
#include "bank.h"
#include "request.h"

using namespace std;

Bank::Bank(
		int k ,
		void (*_display)(int , float , float) ,
		void (*_stopUI)(int)){
	krona = k;
	initKrona = k;
	stopUI = _stopUI;
	limitPayments = 0;
	display = _display;
	pthread_mutex_init(&queLock, NULL);
	pthread_mutex_init(&krnLock, NULL);
	pthread_mutex_init(&pltLock, NULL);
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
	static int count = 0;
	// protect krona?
	fprintf(stderr, "req %d payback.\n", req -> idx);
	pthread_mutex_lock(&pltLock);
	pthread_mutex_lock(&krnLock);
	count++;
	krona += amount;
	for(int i = 0; i < personList.size(); i++){
		if(req -> idx == personList[i] -> idx){
			personList.erase(personList.begin() + i);
			fprintf(stderr, "remove %d from personList\n", personList[i] -> idx);
			break;
		}
	}
	display(-1 , krona , initKrona);
	if(limitPayments && count >= limitPayments){
		stopUI(0);
	}
	pthread_mutex_unlock(&krnLock);
	pthread_mutex_unlock(&pltLock);
	fprintf(stderr, "exit getPayment\n");
}


bool cmp(Request* a, Request* b){
	int aleft = a -> quota - a -> krona, bleft = b -> quota - b -> krona;
	return aleft < bleft || (aleft == bleft && a -> quota > b -> quota);
}

bool Bank :: reqCheck(pair<Request*, int> newRequest){
		sort(personList.begin(), personList.end(), cmp);
		int remain = krona - newRequest.second;
		//fprintf(stderr, "remain = %d krona = %d newRequest.second = %d\n", remain, krona, newRequest.second);
		//check whether the bank can accept this request or not
		if(remain >= newRequest.first -> quota - (newRequest.first -> krona + newRequest.second)){
			//fprintf(stderr, "idx = %d remain = %d newRequestLeft = %d quota = %d\n", newRequest.first -> idx, remain, 
					//newRequest.first -> quota - (newRequest.first -> krona + newRequest.second), newRequest.first -> quota);
			return true;
		}
		for(int i = 0; i < personList.size(); i++){
				if(remain >= newRequest.first -> quota - (newRequest.first -> krona + newRequest.second)){
					//fprintf(stderr, "idx = %d remain = %d newRequestLeft = %d quota = %d\n", newRequest.first -> idx, remain, 
					//newRequest.first -> quota - (newRequest.first -> krona + newRequest.second), newRequest.first -> quota);
					return true;
				}
				else if(remain < personList[i] -> quota - personList[i] -> krona)
					return false;
				//fprintf(stderr, "remain = %d personList[%d] -> krona = %d\n", remain, i, personList[i] -> krona);
				remain += personList[i] -> krona;
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
			pthread_mutex_lock(&(self -> krnLock));
			if(self -> reqCheck(queue[0])){
				bool flag = false;
				for(int i = 0; i < persons.size(); i++)
					if(queue[0].first -> idx == persons[i] -> idx)
						flag = true;
				if(flag == false){
					pthread_mutex_lock(&(self -> pltLock));
					persons.push_back(queue[0].first);
					pthread_mutex_unlock(&(self -> pltLock));
				}

				((queue)[0].first) -> addKrona(
					(queue)[0].second);
				self -> krona -= (queue)[0].second;
				(queue).erase((queue).begin());
				self -> display(-1 , self -> krona , self -> initKrona);
			}
			else{
				queue.push_back(queue[0]);
				queue.erase(queue.begin());
			}

			pthread_mutex_unlock(&(self -> krnLock));
		}

		pthread_mutex_unlock(&(self -> queLock));

		if(self -> krona < (MAX_QUOTA / MIN_QUOTA) + 5){
			self -> display(-1 , self -> krona , self -> initKrona);
			self -> stopUI(1);
		}
	}
	return ptr;
}
