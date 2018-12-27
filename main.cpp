/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include <iostream>
#include <vector>
#include "/usr/include/mpi/mpi.h"
#include "command.h"

void showState(MPI_Comm* parent)
{
    int command = 2;
    MPI_Send(&command, 1, MPI_INT, 0, 0, *parent);
}

void pause(MPI_Comm* parent)
{
    int command = 0;
    MPI_Send(&command, 1, MPI_INT, 0, 0, *parent);
}

void resume(MPI_Comm* parent)
{
    int command = 0;
    MPI_Send(&command, 1, MPI_INT, 0, 0, *parent);
}

void stop(MPI_Comm* parent)
{

}

void addCar(MPI_Comm* parent)
{
    int command = 1;
    MPI_Send(&command, 1, MPI_INT, 0, 0, *parent);
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    bool exit = false;
    Command* commandInterpreter = new Command();
    command_t command;
    command.args = new std::vector<char*>();

    srand(time(NULL));

    MPI_Comm* parent;
    MPI_Comm_get_parent(parent);

    while(!exit)
    {
        command.clean();
        std::cout << "\n$: ";
        commandInterpreter->readCommand(&command);
        switch(command.type)
        {
            case PAUSE:
                pause(parent);
                break;
            case RESUME:
                resume(parent);
                break;
            case ADD_CARS:
                addCar(parent);
                break;
            case SHOW_STATE:
                showState(parent);
                break;
            case END:
                //stop
                break;
            case NO_COMMAND:
                std::cout << "Command not found\n";
                break;
            default:
                std::cout << "Command not found\n";
                break;
        }
    }

    delete commandInterpreter;
    delete command.args;
    return 0;
}

