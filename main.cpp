#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <thread>//sleep until
#include "parking.h"
#include "car.h"
#include "command.h"

#define NUM_THREADS 20
using namespace std;

void showState(std::vector<Car*> carsOnVector, Parking* parking)
{
    std::cout << "-----ESTADO DE PLAZAS-----------------------------------" << std::endl;
    int numPlace = 1;
    for(auto place : parking->getPlaces())
    {
        std::cout << "Plaza " << numPlace << ": ";
        if(place == -1)
        {
            std::cout << "LIBRE" << std::endl;
        }
        else
        {
            std::cout << place << std::endl;
        }
        numPlace++;
    }
    std::cout << "-----COCHES EN ESPERA-----------------------------------" << std::endl;
    for(Car* car : carsOnVector)
    {
        if(car->getState() == waiting)
        {
            std:: cout << car->getThreadId() << std:: endl;
        }
    }
    std::cout << "-----COCHES EN CARRETERA--------------------------------" << std::endl;
    for(Car* car : carsOnVector)
    {
        if(car->getState() == freed)
        {
            std:: cout << car->getThreadId() << std:: endl;
        }
    }
}

void pause(std::vector<Car*> carsOnVector)
{
    for(Car* car : carsOnVector)
    {
        car->setPause(true);
    }
}

void resume(std::vector<Car*> carsOnVector)
{
    for(Car* car : carsOnVector)
    {
        car->setPause(false);
    }
}

int main( int argc, const char* argv[] )
{
    //    omp_set_nested(1);
        srand(time(NULL));
        bool exit = false;
        std::vector<Car*> carsOnVector;
        Parking* parking = new Parking();
        Command* commandInterpreter = new Command();
        command_t command;
        Car* tempCar;
    /*#pragma omp parallel for private(tempCar) num_threads(NUM_THREADS)
    for(int i=0;i<NUM_THREADS;i++){

        tempCar=new Car(omp_get_thread_num() , parking);

        carsOnVector.push_back(tempCar);

        tempCar->startEngine();

    }*/
#pragma omp parallel num_threads(NUM_THREADS)
    {


if(omp_get_thread_num()!=0){
    tempCar=new Car(omp_get_thread_num() , parking);

    carsOnVector.push_back(tempCar);

    tempCar->startEngine();
    }
else{


        std:cout<<"HOLAAAA"<<std::endl;
    command.args = new std::vector<char*>();


    while(!exit){
        command.clean();
        std::cout << "$: ";
        commandInterpreter->readCommand(&command);
        switch(command.type)
        {
            case PAUSE:
                pause(carsOnVector);
                break;
            case RESUME:
                resume(carsOnVector);
                break;
            case ADD_CARS:
    //#pragma omp section
                carsOnVector.push_back(new Car(omp_get_thread_num() , parking));

                break;
            case SHOW_STATE:
                showState(carsOnVector, parking);
                break;
            case END:
                exit = true;
                break;
            case NO_COMMAND:
                std::cout << "Command not found" << std::endl;
                break;
            default:
                std::cout << "Command not found" << std::endl;
                break;

}




    for(Car* car : carsOnVector)
    {
        car->stopEngine();
        delete(car);
    }
    delete(parking);


}
}


}
    return 0;
}

