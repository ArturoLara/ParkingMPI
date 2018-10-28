/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include <omp.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#pragma once

#include "car.h"

class Car;
//the semafor class
using namespace std::chrono;

class Parking
{
private:
    int numPlaces;
    int countRegisteredCars;
    bool closeParking;
    bool pause;
    std::vector<int> places;
    std::vector<time_t> joinTime;

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

    void close();

    void ORASystem();

    void setPause(bool state);

    bool getPause();

};

