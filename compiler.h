#include "command.h"
#ifndef RESTORAN_H
#define RESTORAN_H


class Compiler
{
private:
    int count_of_commands;
    Command **commands;
public:
    static bool cmp(char* str);
    Compiler(int);
    ~Compiler();
    bool getCommand(char*);
};

#endif // COMPILER_H
