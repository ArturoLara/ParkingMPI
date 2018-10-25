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
int getRandomTime(){
    int randNum = rand()%(3-0 + 1) + 0;

    return randNum;
}

void car(Parking parking){
    int state=freed;
    int time;
    while(true){
        case state:
            if(state==waiting){
                time=getRandomTime();
                sleep_until(system_clock::now() + seconds(time));
            }
        case waiting:
            //lock()????
            //hacer funcion para obtener y disminuir plazas
            if(parking.freePlaces>0){
                parking.freePlaces--;
                state=parked;

            }
            //unlock()
            if(state==waiting)
                //sleep()
        case parked:
                time=getRandomTime();
                sleep_until(system_clock::now() + seconds(time));

    }
}
int main( int argc, const char* argv[] )
{


    cout<< "This is Arturo's and Mario-sama's main" <<endl;
    Parking* parking=new Parking();
    coche(parking);
    cout<<parking->anyFree()<<endl;

    return 0;
}
