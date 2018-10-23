#include "car.h"


Car::Car(int threadId, Parking parking)
{

    this->threadId=threadId;
    this->state=freed;
    this->parking=parking;
}

void Car::standBy(){

    this->state=waiting;
}
void Car::startEngine(){

}
void Car::park(){
    this->state=parked;
}
void Car::askForParking(){

}
