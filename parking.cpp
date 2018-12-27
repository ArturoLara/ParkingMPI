/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include "/usr/include/mpi/mpi.h"

const int PLACES = 10;
const int NUM_CARS = 20;

using namespace std::chrono;

void showState(std::vector<int>* places, std::vector<int>* onRoad, std::vector<int>* waitting)
{
    std::cout << "-----ESTADO DE PLAZAS-----------------------------------\n";
    int numPlace = 1;
    for(int place : *places)
    {
        std::cout << "Plaza " << numPlace << ": ";
        if(place == -1)
        {
            std::cout << "LIBRE\n";
        }
        else
        {
            std::cout << place << "\n";
        }
        numPlace++;
    }
    std::cout << "-----COCHES EN ESPERA-----------------------------------\n";
    for(int car : *waitting)
    {
        std::cout << car << "\n";
    }
    std::cout << "-----COCHES EN CARRETERA--------------------------------\n";
    for(int car : *onRoad)
    {
        std::cout << car << "\n";
    }
}

int getFreePlace(std::vector<int>* places)
{

    for(int i=0; i < PLACES; i++){
        if((*places)[i] == -1)
            return i;
    }
    return -1;
}

int getPlaceOfThread(std::vector<int>* places, int theadId)
{

    for(int i=0; i < PLACES; i++){
        if((*places)[i] == theadId)
            return i;
    }
    return -1;
}

void registerNewCar(std::vector<int>* onRoad, std::vector<MPI_Comm*>* cars)
{
    int threadID = cars->size();
    MPI_Comm* commCar = new MPI_Comm[1];

    MPI_Comm_spawn("car", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, commCar, MPI_ERRCODES_IGNORE);
    MPI_Send(&threadID, 1, MPI_INT, 0, 0, *commCar);
    cars->push_back(commCar);
    onRoad->push_back(threadID);
}

void  ocuppyAPlace(int threadId, std::vector<MPI_Comm*>* cars, std::vector<int>* places, std::vector<int>* onRoad, std::vector<int>* waitting, int* numPlaces, time_t* joinTime)
{
    if((*numPlaces)>0)
    {
        (*numPlaces)--;
        int freePlace = getFreePlace(places);
        if(freePlace != -1)
        {
            joinTime[freePlace] = system_clock::to_time_t(system_clock::now());
            (*places)[freePlace] = threadId;
            onRoad->erase(std::remove(onRoad->begin(), onRoad->end(), threadId), onRoad->end());
        }
        else
        {
            std::cout << "ERROR: async threads, park in the same place" << std::endl;
        }
    }
    else
    {
        onRoad->erase(std::remove(onRoad->begin(), onRoad->end(), threadId), onRoad->end());
        waitting->push_back(threadId);
    }
}
void leaveAPlace(int threadId, std::vector<MPI_Comm*>* cars, std::vector<int>* places, std::vector<int>* onRoad, std::vector<int>* waitting, int* numPlaces, time_t* joinTime)
{

    int idPlace = getPlaceOfThread(places, threadId);
    if(idPlace != -1)
    {
        (*numPlaces)++;
        (*places)[idPlace] = -1;
        joinTime[idPlace] = static_cast<time_t>(0);
        onRoad->push_back(threadId);
        if(waitting->size() > 0)
        {
            ocuppyAPlace(waitting->front(), cars, places, onRoad, waitting, numPlaces, joinTime);
            waitting->erase(waitting->begin());
        }
    }
}

void leaveAPlaceWithPosition(int placeID, std::vector<MPI_Comm*>* cars, std::vector<int>* places, std::vector<int>* onRoad, std::vector<int>* waitting, int* numPlaces, time_t* joinTime)
{
    (*numPlaces)++;
    (*places)[placeID] = -1;
    joinTime[placeID] = static_cast<time_t>(0);
    onRoad->push_back((*places)[placeID]);
    if(waitting->size() > 0)
    {
        ocuppyAPlace(waitting->front(), cars, places, onRoad, waitting, numPlaces, joinTime);
        waitting->erase(waitting->begin());
    }
}

int main(int argc, char** argv)
{
    int numPlaces = PLACES;
    int func = -1;
    int nullValue = 0;
    int msgSize=0;
    int * listOfCarsToOut;
    bool closeParking = false;

    MPI_Init(&argc, &argv);
    MPI_Status status;

    std::vector<MPI_Comm*> cars;
    std::vector<int> places;
    std::vector<int> onRoad;
    std::vector<int> waitting;
    time_t* joinTime;
    MPI_Comm terminal;
    MPI_Comm ora;

    MPI_Comm* commCar = new MPI_Comm[1];
    for(int i = 0; i < NUM_CARS; i++)
    {
        MPI_Comm_spawn("car", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, commCar, MPI_ERRCODES_IGNORE);
        cars.push_back(commCar);
    }

    joinTime = new time_t[PLACES];
    for(int i = 0; i < PLACES ; i++){
        places.push_back(-1);
        joinTime[i] = static_cast<time_t>(0);
    }

    //MPI_Comm_spawn("terminal", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &terminal, MPI_ERRCODES_IGNORE);

    //MPI_Comm_spawn("ora", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &ora, MPI_ERRCODES_IGNORE);
    //MPI_Send(&PLACES, 1, MPI_INT, 0, 0, ora);
    while(!closeParking)
    {
        int flag = false;
        //MPI_Iprobe(0, 0, terminal, &flag, &status);
        if(flag)
        {
            std::cout << "mensaje entrante: terminal" << std::endl;
            MPI_Recv(&func, 1, MPI_INT, 0, 0, terminal, &status);
            switch (func) {
            case 0:
                MPI_Recv(&nullValue, 1, MPI_INT, 0, 0, terminal, &status);
                break;
            case 1:
                registerNewCar(&onRoad, &cars);
                break;
            case 2:
                showState(&places, &onRoad, &waitting);
                break;
            default:
                break;
            }
            func = -1;
            flag = false;
        }

        //MPI_Iprobe(0, 0, ora, &flag, &status);
        if(flag)
        {
            std::cout << "mensaje entrante: ora" << std::endl;
            msgSize=0;

            MPI_Get_count(&status, MPI_CHAR, &msgSize);
            listOfCarsToOut = new int[msgSize];
            MPI_Recv(listOfCarsToOut, msgSize, MPI_CHAR, 0, 0, ora, &status);

            for(int i = 0; i < msgSize; i++)
            {
                leaveAPlaceWithPosition(listOfCarsToOut[i], &cars, &places, &onRoad, &waitting, &numPlaces, joinTime);
            }
            //MPI_Send((void*)joinTime, sizeof(time_t)*PLACES, MPI_CHAR, 0, 0, ora);

            delete listOfCarsToOut;
            flag = false;
        }
        int msgFound = -1;
        for(int i = 0; i < cars.size() && (msgFound == -1); i++)
        {
            MPI_Iprobe(0, 0, *(cars[i]), &flag, &status);
            if(flag)
                msgFound=i;
        }
        if(msgFound!=-1)
        {
            std::cout << "mensaje entrante: coche -> " << msgFound << std::endl;
            MPI_Recv(&func, 1, MPI_INT, 0, 0, *(cars[msgFound]), &status);
            switch (func) {
            case 0:
                ocuppyAPlace(msgFound, &cars, &places, &onRoad, &waitting, &numPlaces, joinTime);
                break;
            case 1:
                leaveAPlace(msgFound, &cars, &places, &onRoad, &waitting, &numPlaces, joinTime);
                break;
            default:
                break;
            }
            func = -1;
            flag = false;
            msgFound = -1;
        }
    }
    return 0;
}

