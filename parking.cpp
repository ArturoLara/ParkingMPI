#include "parking.h"
#include <time.h>
#define PLACES 10

Parking::Parking()
{

    this->numPlaces=PLACES;
    omp_init_lock(&locker);

    for(int i=0; i< PLACES ;i++){
        places.push_back(-1);
    }
}

Parking::~Parking()
{
    omp_destroy_lock(&locker);
}


int  Parking::ocuppyAPlace(int threadId){
    if(numPlaces>0)
    {
        numPlaces--;
        int freePlace = getFreePlace();
        if(freePlace != -1)
        {
            places[freePlace] = threadId;
            //std::cout << "APARCAO. AUREEEEEEEEELIO   =  " <<threadId<< std::endl;
          /*  if(threadId%2==0){
                  time_t     now;
                  time(&now);
                  struct tm  tstruct;

                      tstruct = *localtime(&now);
                std::cout<<threadId<<" "<<tstruct.tm_sec<<std::endl;
            }*/
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

    numPlaces++;
    int idPlace = getPlaceOfThread( threadId );
    if(idPlace != -1)
    {
        places[idPlace] = -1;
        //std::cout<<"Aurelio se va :'(  = "<<threadId<<std::endl;
        /*if(threadId%2==0){
             time_t     now;
             time(&now);
             struct tm  tstruct;

                 tstruct = *localtime(&now);
            std::cout<<threadId<<" "<<tstruct.tm_sec<<std::endl;
        }*/
        return 0;
    }
    else
    {
        std::cout << "ERROR: async threads, is not parked" << std::endl;
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


