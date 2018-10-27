#ifndef PARKING_H
#define PARKING_H

#include <omp.h>
#include <vector>
#include <iostream>

//the semafor class
class Parking
{
private:
    int numPlaces;
    int countRegisteredCars;
    std::vector<int> places;

    //function to get the id of a free place
    int getFreePlace();
    //function to get the id place where thread is parked
    int getPlaceOfThread(int theadId);

public:
    omp_lock_t locker;

    Parking();
    ~Parking();
    //function to ocuppy a place of the parking
    int ocuppyAPlace(int threadId);
    //function to leave a place of the parking
    int leaveAPlace(int threadId);
    //function to get the palces
    std::vector<int> getPlaces();

    int registerCar();


};

#endif // PARKING_H
