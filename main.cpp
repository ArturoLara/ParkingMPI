/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <thread>//sleep until
#include "parking.h"
#include "car.h"
#include "command.h"

#define NUM_CARS 20
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

void pause(std::vector<Car*> carsOnVector, Parking* parking)
{
    parking->setPause(true);
    for(Car* car : carsOnVector)
    {
        car->setPause(true);
    }
}

void resume(std::vector<Car*> carsOnVector, Parking* parking)
{
    parking->setPause(false);
    for(Car* car : carsOnVector)
    {
        car->setPause(false);
    }
}

void stop(std::vector<Car*> carsOnVector)
{
    for(Car* car : carsOnVector)
    {
        car->stopEngine();
    }
}

Car* addCar(Parking* parking, std::vector<Car*>* carsOnVector)
{
    Car* tempCar;

    tempCar = new Car(parking);
    carsOnVector->push_back(tempCar);

    return tempCar;
}

bool menu(Command* commandInterpreter, command_t* command, std::vector<Car*>* carsOnVector, Parking* parking)
{
    bool exit = false;
    Car* car;

    while(!exit){
        command->clean();
        std::cout << "$: ";
        commandInterpreter->readCommand(command);
        switch(command->type)
        {
            case PAUSE:
                pause(*carsOnVector, parking);
                break;
            case RESUME:
                resume(*carsOnVector, parking);
                break;
            case ADD_CARS:
                car = addCar(parking, carsOnVector);
                #pragma omp parallel sections num_threads(2)
                {
                    #pragma omp section
                    car->startEngine();
                    #pragma omp section
                    exit = menu(commandInterpreter, command, carsOnVector, parking);
                }
                break;
            case SHOW_STATE:
                showState(*carsOnVector, parking);
                break;
            case END:
                stop(*carsOnVector);
                parking->close();
                exit = true;
                break;
            case NO_COMMAND:
                std::cout << "Command not found" << std::endl;
                break;
            default:
                std::cout << "Command not found" << std::endl;
                break;
        }
    }
    return exit;
}

int main()
{
        omp_set_nested(1);
        omp_set_dynamic(0);
        srand(time(NULL));

        std::vector<Car*> carsOnVector;
        Parking* parking = new Parking();
        Car* tempCar;

#pragma omp parallel shared(parking, carsOnVector) firstprivate(tempCar) num_threads(NUM_CARS+2)
    {
        if(omp_get_thread_num() > 1)
        {
            tempCar = new Car(parking);
            carsOnVector.push_back(tempCar);
            tempCar->startEngine();
        }
        else if(omp_get_thread_num() == 1)
        {
            parking->ORASystem();
        }
        else
        {
            Command* commandInterpreter = new Command();
            command_t command;
            command.args = new std::vector<char*>();

            menu(commandInterpreter, &command, &carsOnVector, parking);
        }
#pragma omp barrier
#pragma omp single
        {
            for(Car* car : carsOnVector)
            {
                delete(car);
            }
            delete(parking);
        }
    }
        return 0;
}

