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

void stop(std::vector<Car*> carsOnVector)
{
    for(Car* car : carsOnVector)
    {
        car->stopEngine();
    }
}

Car* addCar(Parking* parking)
{
    Car* tempCar;

    tempCar = new Car(parking);
    tempCar->startEngine();

    return tempCar;
}

void addCar(std::vector<Car*>* carsOnVector, Parking* parking)
{
    Car* tempCar = new Car(parking);
    carsOnVector->push_back(tempCar);
    tempCar->startEngine();
}

int main()
{
        omp_set_nested(1);
        srand(time(NULL));

        std::vector<Car*> carsOnVector;
        Parking* parking = new Parking();
        Car* tempCar;

#pragma omp parallel shared(parking, carsOnVector) firstprivate(tempCar) num_threads(2)
    {
        if(omp_get_thread_num() > 0)
        {
#pragma omp for nowait
            for(int i = 0; i < 20; i++)
            {
                addCar(&carsOnVector, parking);
            }
        }
        else
        {
            std::cout << omp_get_num_threads() << " despues" << std::endl;
            std::cout << omp_get_thread_num() << std::endl;
            bool exit = false;
            Command* commandInterpreter = new Command();
            command_t command;
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
                        carsOnVector.push_back(addCar(parking));
                        break;
                    case SHOW_STATE:
                        showState(carsOnVector, parking);
                        break;
                    case END:
                        stop(carsOnVector);
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
        std::cout << "I'm free mother fucker! thread " << omp_get_thread_num() << " OUT" << std::endl;
    }


    return 0;
}

