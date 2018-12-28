#include <vector>
#include <chrono>
#include <thread> //sleep until
#include "/usr/include/mpi/mpi.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define ORA_TIME 5
#define MAX_PARKING_TIME 2

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    bool closeParking = false;
    int numPlaces;
    time_t* carTimes;
    std::vector<int> carsToLeave;

    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    MPI_Status status;

    MPI_Recv(&numPlaces, 1, MPI_INT, 0, 0, parent, &status);

    carTimes = new time_t[numPlaces];
    for(int i = 0; i < numPlaces; i++)
    {
        carTimes[i] = static_cast<time_t>(0);
    }

    while(!closeParking)
    {
        int func;
        bool ready = false;

        std::this_thread::sleep_until(system_clock::now() + seconds(ORA_TIME));
        MPI_Send(&numPlaces, 1, MPI_INT, 0, 0, parent);
        MPI_Recv(&func, 1, MPI_INT, 0, 0, parent, &status);
        switch (func) {
        case 0:
            MPI_Recv(&func, 1, MPI_INT, 0, 0, parent, &status);
            break;
        case 2:
            closeParking = false;
            break;
        default:
            ready = true;
            break;
        }
        if(!closeParking && ready)
        {
            MPI_Recv(carTimes, sizeof(time_t)*numPlaces, MPI_CHAR, 0, 0, parent, &status);
            for(int i = 0; i < numPlaces; i++)
            {
                if(carTimes[i] != static_cast<time_t>(0))
                {
                    if((system_clock::to_time_t(system_clock::now()) - carTimes[i]) >= MAX_PARKING_TIME)
                    {
                        carsToLeave.push_back(i);
                    }
                }
            }
            MPI_Send((void*)carsToLeave.data(), sizeof(int)*carsToLeave.size(), MPI_CHAR, 0, 0, parent);
            carsToLeave.clear();
            ready = false;
        }
    }
    return 0;
}
