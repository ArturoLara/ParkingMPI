#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <iostream>
#include <cstring>

enum command_e
{
    NO_COMMAND = 0,
    PAUSE = 1,
    SHOW_STATE = 2,
    ADD_CARS = 3,
    RESUME = 4,
    END = -1
};

typedef struct command_t{
    command_e type;
    std::vector<char*>* args;

    void clean()
    {
        type = command_e::NO_COMMAND;
        args->clear();
    }
}command_t;

class Command
{
public:
    Command();
    void readCommand(command_t* aCommand);
private:
    command_e getTypeOfCommand(char* aCommandArray);
};

#endif // COMMAND_H
