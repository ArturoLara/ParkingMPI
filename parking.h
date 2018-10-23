#ifndef PARKING_H
#define PARKING_H

//the semafor class
class Parking
{
private:
    bool *freePlaces;
    int numPlaces;
public:
    Parking();
    //function to ocuppy a place of the parking
    void ocuppyAPlace();
    //function to leave a place of the parking
    void leaveAPlace();
    //function to check if there are any free places
    bool anyFree();

};

#endif // PARKING_H
