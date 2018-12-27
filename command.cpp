/*
 * Authors: Mario Cavero y Arturo Lara
*/

#include "command.h"

Command::Command() { }

void Command::readCommand(command_t* aCommand){
    char * line= new char [1024];
    char spacer[3]=" \n";
    char* token=NULL;
    fgets(line, 1023, stdin);
    token=strtok(line, spacer);
    aCommand->type=getTypeOfCommand(token);

    while(token!=NULL){
        token=strtok(NULL, spacer);
        aCommand->args->push_back(token);
    }
}

command_e Command::getTypeOfCommand(char* aCommandArray)
{
    if(aCommandArray != NULL)
    {
        if(!strncmp("pause", aCommandArray, 5))
            return command_e::PAUSE;
        else if(!strncmp("addCar", aCommandArray, 6))
            return command_e::ADD_CARS;
        else if(!strncmp("showState", aCommandArray, 9))
            return command_e::SHOW_STATE;
        else if(!strncmp("resume", aCommandArray, 9))
            return command_e::RESUME;
        else if(!strncmp("exit", aCommandArray, 4))
            return command_e::END;
    }
    return command_e::NO_COMMAND;
}

