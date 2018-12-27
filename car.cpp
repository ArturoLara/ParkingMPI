/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include <chrono> //system_clock
#include <thread> //sleep until
#include "/usr/include/mpi/mpi.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


enum State{
    freed=0,
    parked=1

};

void park(MPI_Comm parent, State* state)
{
    int function = 0;
    std::cout << function << std::endl;
    MPI_Status status;
    MPI_Send(&function, 1, MPI_INT, 0, 0, parent);
    MPI_Recv(&function, 1, MPI_INT, 0, 0, parent, &status);
    *state = parked;

}

void goToRoad(MPI_Comm parent, State* state)
{
    int function = 1;
    std::cout << function << std::endl;
    MPI_Send(&function, 1, MPI_INT, 0, 0, parent);
    *state = freed;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm parent;
    MPI_Status status;
    MPI_Comm_get_parent(&parent);

    State state = freed;
    bool engine = true;

    int time;

    while(engine)
    {
        int flag, func, nullValue;
        MPI_Iprobe(0, 0, parent, &flag, &status);
        if(flag)
        {
            std::cout << "mensaje entrando" << std::endl;
            MPI_Recv(&func, 1, MPI_INT, 0, 0, parent, &status);
            switch (func) {
            case 0:
                MPI_Recv(&nullValue, 1, MPI_INT, 0, 0, parent, &status);
                break;
            case 1:
                engine = false;
                break;
            default:
                break;
            }
            func = -1;
            flag = false;
        }
        if(engine)
        {
            time=rand()%4;
            std::this_thread::sleep_until(system_clock::now() + seconds(time));

            if(state == freed)
            {
                park(parent, &state);
            }
            else if(state == parked)
            {
                //goToRoad(parent, &state);
            }
        }
    }
    return 0;
}
