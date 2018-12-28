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
    cars->push_back(commCar);
    onRoad->push_back(threadID);
}

void  ocuppyAPlace(int threadId, std::vector<MPI_Comm*>* cars, std::vector<int>* places, std::vector<int>* onRoad, std::vector<int>* waitting, int* numPlaces, time_t* joinTime)
{

    if((*numPlaces)>0)
    {
        (*numPlaces)--;
        int freePlace = getFreePlace(places);
        int unlockMsg = 2;
        if(freePlace != -1)
        {
            joinTime[freePlace] = system_clock::to_time_t(system_clock::now());
            (*places)[freePlace] = threadId;
            MPI_Send(&unlockMsg, 1, MPI_INT, 0, 0, *(cars->at(threadId)));
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
    joinTime[placeID] = static_cast<time_t>(0);
    onRoad->push_back((*places)[placeID]);
    (*places)[placeID] = -1;
    if(waitting->size() > 0)
    {
        ocuppyAPlace(waitting->front(), cars, places, onRoad, waitting, numPlaces, joinTime);
        waitting->erase(waitting->begin());
    }
}

void pause(std::vector<MPI_Comm*> *cars, MPI_Comm ora, bool *pause)
{
    int function = 0;
    *pause = !(*pause);
    for(int i = 0; i < cars->size(); i++)
    {
        MPI_Send(&function, 1, MPI_INT, 0, 0, *((*cars)[i]));
    }
    MPI_Send(&function, 1, MPI_INT, 0, 0, ora);
}

void stop(MPI_Comm ora, std::vector<MPI_Comm*>* cars, bool* closeParking, bool *pause)
{
    int function = 2;
    *closeParking = true;
    *pause = true;
    for(int i = 0; i < cars->size(); i++)
    {
        MPI_Send(&function, 1, MPI_INT, 0, 0, *((*cars)[i]));
    }
    MPI_Send(&function, 1, MPI_INT, 0, 0, ora);

}

int main(int argc, char** argv)
{
    int numPlaces = PLACES;
    int func = -1;
    int msgSize=0;
    int * listOfCarsToOut;
    bool closeParking = false;
    bool pauseFlag = false;

    MPI_Init(&argc, &argv);
    MPI_Status status;

    std::vector<MPI_Comm*> cars;
    std::vector<int> places;
    std::vector<int> onRoad;
    std::vector<int> waitting;
    time_t* joinTime;
    MPI_Comm terminal;
    MPI_Comm ora;

    for(int i = 0; i < NUM_CARS; i++)
    {
        MPI_Comm* commCar = new MPI_Comm[1];
        MPI_Comm_spawn("car", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, commCar, MPI_ERRCODES_IGNORE);
        cars.push_back(commCar);
        onRoad.push_back(i);
    }

    joinTime = new time_t[PLACES];
    for(int i = 0; i < PLACES ; i++){
        places.push_back(-1);
        joinTime[i] = static_cast<time_t>(0);
    }

    MPI_Comm_spawn("terminal", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &terminal, MPI_ERRCODES_IGNORE);

    MPI_Comm_spawn("ora", MPI_ARGV_NULL, 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &ora, MPI_ERRCODES_IGNORE);
    MPI_Send(&numPlaces, 1, MPI_INT, 0, 0, ora);
    while(!closeParking)
    {
        int flag = 0;
        MPI_Iprobe(0, 0, terminal, &flag, &status);
        if(flag)
        {
            MPI_Recv(&func, 1, MPI_INT, 0, 0, terminal, &status);
            switch (func) {
            case 0:
                pause(&cars, ora, &pauseFlag);
                break;
            case 1:
                registerNewCar(&onRoad, &cars);
                break;
            case 2:
                showState(&places, &onRoad, &waitting);
                break;
            case 3:
                stop(ora, &cars, &closeParking, &pauseFlag);
                break;
            default:
                break;
            }
            func = -1;
            flag = 0;
        }
        if(pauseFlag)
        {
            MPI_Iprobe(0, 0, ora, &flag, &status);
            if(flag)
            {
                MPI_Recv(&flag, 1, MPI_INT, 0, 0, ora, &status);
                flag = 0;
            }
        }
        if(!pauseFlag)
        {
            MPI_Iprobe(0, 0, ora, &flag, &status);
            if(flag)
            {
                int func = 1;
                MPI_Recv(&flag, 1, MPI_INT, 0, 0, ora, &status);
                MPI_Send(&func, 1, MPI_INT, 0, 0, ora);
                MPI_Send((void*)joinTime, sizeof(time_t)*PLACES, MPI_CHAR, 0, 0, ora);
                msgSize=0;
                MPI_Probe(0, 0, ora, &status);
                MPI_Get_count(&status, MPI_CHAR, &msgSize);
                listOfCarsToOut = new int[(msgSize/sizeof(int))];
                MPI_Recv(listOfCarsToOut, msgSize, MPI_CHAR, 0, 0, ora, &status);
                if(msgSize > 0)
                {
                    for(int i = 0; i < (msgSize/sizeof(int)); i++)
                    {
                        std::cout << "El ORA ha expulsado al coche: " << places[listOfCarsToOut[i]] << "\n";
                        leaveAPlaceWithPosition(listOfCarsToOut[i], &cars, &places, &onRoad, &waitting, &numPlaces, joinTime);
                    }
                    std::cout << "\n";
                    delete listOfCarsToOut;
                }

                flag = 0;
            }
            int msgFound = -1;
            for(int i = 0; i < cars.size() && (msgFound == -1); i++)
            {
                MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, *(cars[i]), &flag, &status);
                if(flag)
                    msgFound=i;
            }
            if(msgFound!=-1)
            {
                MPI_Recv(&func, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, *(cars[msgFound]), &status);
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
                flag = 0;
                msgFound = -1;
            }
        }
    }
    MPI_Finalize();
    return 0;
}

