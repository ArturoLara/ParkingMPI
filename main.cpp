#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <thread>//sleep until
#include "parking.h"
#include "car.h"
#include "command.h"


using namespace std;

int getRandomTime(){//WHAT IZ THIZ?! NANI!!
    int randNum = rand()%(3-0 + 1) + 0;

    return randNum;
}

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
    srand(time(NULL));
    bool exit = false;
    std::vector<Car*> carsOnVector;
    cout<< "This is Arturo's and Mario-sama's main" <<endl;
    Parking* parking = new Parking();
    Command* commandInterpreter = new Command();
    command_t command;

#pragma omp parallel sections num_threads(20)
{
    #pragma omp section
        {
    carsOnVector.push_back(new Car(omp_get_thread_num() , parking));


    command.args = new std::vector<char*>();

    while(!exit){
        command.clean();
        std::cout << "$: ";
        commandInterpreter->readCommand(&command);
        switch(command.type)
        {
            case command_e::PAUSE:
                pause(carsOnVector);
                break;
            case command_e::RESUME:
                resume(carsOnVector);
                break;
            case command_e::ADD_CARS:
                break;
        }
    }
        }
    #pragma omp section
        {
                carsOnVector.push_back(new Car(omp_get_thread_num() , parking));
                break;
            case command_e::SHOW_STATE:
                showState(carsOnVector, parking);
                break;
            case command_e::END:
                exit = true;
                break;
            case command_e::NO_COMMAND:
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
    return 0;
}

