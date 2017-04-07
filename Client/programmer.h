#ifndef CLIENT_H
#define CLIENT_H


#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#define FIFO_NAME "./fifofile"

class Programmer
{
private:

    FILE* pipe;

public:

    Programmer();

    ~Programmer();

    bool connectToPipe(char* pipePath);

    void runCompile(char* program);

    void closeConnectionToPipe();

};

#endif // CLIENT_H
