#pragma once

#include "parking.h"
#include <omp.h>
#include <chrono> //system_clock
#include <thread> //sleep until

class Parking;

enum State{
    freed=0,
    waiting=1,
    parked=2

};

class Car
{
private:
    int threadId;
    State state;
    Parking* parking;
    bool pause;
    bool engine;
    bool ticket;
public:
    Car(Parking* parking);
    void standBy();
    void startEngine();
    void park();
    void goToRoad();
    void stopEngine();
    State getState();
    int getThreadId();
    void setPause(bool pause);
    void setTicket();
    bool getTicket();
};

