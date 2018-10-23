#include "parking.h"
#define PLACES 10
Parking::Parking()
{

    this->numPlaces=PLACES;
    this->freePlaces=new bool [this->numPlaces];
    for(int i=0;i<this->numPlaces;i++){
        this->freePlaces[i]=true;
    }
}


void  Parking::ocuppyAPlace(){

}
void Parking::leaveAPlace(){

}
bool Parking::anyFree(){

    for(int i=0;i<this->numPlaces;i++){
        if(this->freePlaces[i]==true)
            return true;
    }
    return false;
}


