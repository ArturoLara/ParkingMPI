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

void park(bool* engine)
{
    int function = 0;
    bool nowait = false;
    MPI_Status status;
    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    MPI_Send(&function, 1, MPI_INT, 0, 0, parent);
    while(!nowait)
    {
        MPI_Recv(&function, 1, MPI_INT, 0, 0, parent, &status);
        switch (function) {
        case 0:
            MPI_Recv(&function, 1, MPI_INT, 0, 0, parent, &status);
            break;
        case 1:
            *engine = false;
            break;
        default:
            nowait = true;
            break;
        }
    }

}

void goToRoad()
{
    int function = 1;
    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    MPI_Send(&function, 1, MPI_INT, 0, 0, parent);
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
                park(&engine);
                state = parked;
            }
            else if(state == parked)
            {
                goToRoad();
                state = freed;
            }
        }
    }
    return 0;
}
