#include "programmer.h"

Programmer::Programmer()
{
    printf("create programmer\n");
}
Programmer::~Programmer()
{

}

bool Programmer::connectToPipe(char* pipeName)
{
    printf("try connect to pipe\n");
    sleep(1);
    this->pipe = fopen(pipeName, "w+");
    printf("connecting to pipe\n");
    sleep(1);
    if(this->pipe == NULL)
    {
        printf("Error! Student cannot connect to pipe!\n");
        return false;
    }
    printf("Client is connected to pipe.\n");
    sleep(1);
    return true;
}

void Programmer::runCompile(char* program)
{
     printf("start sending...\n");
     fputs(program, this->pipe);
     fputs("\0",this->pipe);
     printf("is Send successfully\n");
}

void Programmer::closeConnectionToPipe()
{
    fclose(this->pipe);
    printf("close\n");
}
