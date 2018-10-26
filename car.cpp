#include "car.h"
#include <omp.h>
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

Car::Car(int threadId, Parking* parking)
{

    this->threadId=threadId;
    this->state=freed;
    this->parking=parking;

}

void Car::standBy()
{
    this->state=waiting;
}
void Car::startEngine(){
    int time;
    this->engine = true;

    while(engine)
    {
        while(pause) {;}
        switch(this->state)
        {
        case freed:
            time=rand()%4;
            std::this_thread::sleep_until(system_clock::now() + seconds(time));
            while(pause) {;}
            park();
            break;
        case waiting:
            park();
            break;
        case parked:
            time=rand()%4;
            std::this_thread::sleep_until(system_clock::now() + seconds(time));
            while(pause) {;}
            goToRoad();
            break;
        }
    }
}
void Car::park(){
    omp_set_lock(&(parking->locker));
    int sucess = parking->ocuppyAPlace(threadId);
    omp_unset_lock(&(parking->locker));
    if(sucess == 1)
    {
        standBy();
    }
    else if( sucess == 0)
    {
        state = parked;
    }
}
void Car::goToRoad(){
    omp_set_lock(&(parking->locker));
    int sucess = parking->leaveAPlace(threadId);
    omp_unset_lock(&(parking->locker));
    if(sucess == 0)
    {
        state = freed;
    }
}

State Car::getState()
{
    return state;
}

int Car::getThreadId()
{
    return threadId;
}

void Car::setPause(bool pause)
{
    this->pause = pause;
}

void Car::stopEngine()
{
    engine = false;
}
