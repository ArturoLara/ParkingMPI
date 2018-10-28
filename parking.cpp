#include "parking.h"

#define PLACES 10
#define ORA_TIME 5
#define MAX_PARKING_TIME 2

Parking::Parking()
{

    this->numPlaces=PLACES;
    countRegisteredCars = 0;
    closeParking = false;
    pause = false;
    omp_init_lock(&locker);

    for(int i=0; i< PLACES ;i++){
        places.push_back(-1);
        joinTime.push_back(static_cast<time_t>(0));
    }
}

Parking::~Parking()
{
    omp_destroy_lock(&locker);
}

void Parking::ORASystem()
{
    while(!closeParking)
    {
        std::this_thread::sleep_until(system_clock::now() + seconds(ORA_TIME));
        while(pause){ ; }
        for(int i = 0; i < PLACES; i++)
        {
            if(places[i] != -1)
            {
                int carID = places[i];
                if((system_clock::to_time_t(system_clock::now()) - joinTime[i]) >= MAX_PARKING_TIME)
                {
                    omp_set_lock(&(locker));
                    int sucess = leaveAPlace(carID);
                    omp_unset_lock(&(locker));
                    if(sucess == 0)
                    {
                        std::cout << "\nEl ORA ha expulsado al coche " << carID;
                    }
                }
            }
        }
        std::cout << "\n$: ";
    }
}

void Parking::close()
{
    closeParking = true;
}

void Parking::setPause(bool state)
{
    pause = state;
}

bool Parking::getPause()
{
    return pause;
}

int  Parking::ocuppyAPlace(int threadId){
    if(numPlaces>0)
    {
        numPlaces--;
        int freePlace = getFreePlace();
        if(freePlace != -1)
        {
            joinTime[freePlace] = system_clock::to_time_t(system_clock::now());
            places[freePlace] = threadId;
            return 0;
        }
        else
        {
            std::cout << "ERROR: async threads, park in the same place" << std::endl;
            return -1;
        }
    }
    else
    {
        return 1;
    }
}
int Parking::leaveAPlace(int threadId){

    int idPlace = getPlaceOfThread( threadId );
    if(idPlace != -1)
    {
        numPlaces++;
        places[idPlace] = -1;
        return 0;
    }
    else
    {
        return -1;
    }
}

int Parking::getFreePlace(){

    for(int i=0; i < PLACES; i++){
        if(this->places[i] == -1)
            return i;
    }
    return -1;
}

int Parking::getPlaceOfThread(int theadId){

    for(int i=0; i < PLACES; i++){
        if(this->places[i] == theadId)
            return i;
    }
    return -1;
}

std::vector<int> Parking::getPlaces()
{
    return places;
}

int Parking::registerCar()
{
    return countRegisteredCars++;
}

