#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <chrono>//system_clock
#include <thread>//sleep until
#include "parking.h"
#include "car.h"
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;

int main( int argc, const char* argv[] )
{


    cout<< "This is Arturo's and Mario-sama's main" <<endl;
    Parking* parking=new Parking();
    cout<<parking->anyFree()<<endl;
    //int randNum = rand()%(3-0 + 1) + 0;
    //sleep_until(system_clock::now() + seconds(randNum));
    return 0;
}
