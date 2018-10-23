#ifndef CAR_H
#define CAR_H
#include "parking.h"
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
    Parking parking;
public:
    Car(int threadId, Parking parking);
    void standBy();
    void startEngine();
    void park();
    void askForParking();
};

#endif // CAR_H
