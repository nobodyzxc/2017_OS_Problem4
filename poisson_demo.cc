#include <iostream>
#include <unistd.h>
#include <random>
#include <chrono>
#include <ctime>


using namespace std;
#define PERSENTAGE_PER_SEC 30

int main(){

/*
	int n = 10;

	while(n--){
		cout << n << endl;
		sleep(1);
	}
*/

	default_random_engine generator;
	poisson_distribution<int> distribution(PERSENTAGE_PER_SEC);

	//int n = 10;
	int counter = 0;
	cout << "Poisson process with poisson in 100 seconds with mu = " << PERSENTAGE_PER_SEC << endl;
	while(1){
		sleep(1);
		//cout << distribution(generator) << endl;
		counter += distribution(generator);
		if(counter >= 100){
			counter -= 100;
			auto cur = chrono::system_clock::now();
			time_t cur_time = chrono::system_clock::to_time_t(cur);
			cout << ctime(&cur_time) << "A customer enter.\n" << endl;
		}
	}



	return 0;
}
