#include <math.h>
#include <random>
#include <ctime>
#include <iostream>

using namespace std;

float exp_dist(float lambda){
	//float lambda = 0.1;
	float time = -log(1.0 - (float)( (float)(rand() % 100000) / (float)100000)) / lambda;
	return time;
}
