#include "compiler.h"

Compiler::Compiler(int new_count)
{
    this->count_of_commands = new_count;
    this->commands = new Command*[new_count];
    for (int i = 0; i < new_count; i++)                  // add in compiler supported extensions
    {
        switch(i)
        {
        case 0:
        {
            commands[i] = new Command(".cpp");
            break;
        }
        case 1:
        {
            commands[i] = new Command(".c");
            break;
        }
        case 2:
        {
            commands[i] = new Command(".java");
            break;
        }
        case 3:
        {
            commands[i] = new Command(".py");
            break;
        }
        case 4:
        {
            commands[i] = new Command(".asm");
            break;
        }
        case 5:
        {
            commands[i] = new Command(".class");
            break;
        }
        case 6:
        {
            commands[i] = new Command(".js");
            break;
        }
        case 7:
        {
            commands[i] = new Command(".rb");
            break;
        }
        default:
        {
            commands[i] = new Command(".cpp");
            break;
        }
        }
    }
}
bool Compiler::getCommand(char* name)
{
    for (int i = 0; i < this->count_of_commands; i++)              // check file from extension
    {
        if (commands[i]->equals(name))
        {
            return true;
        }
    }
    return false;
}
Compiler::~Compiler()
{
    for (int i = 0; i < this->count_of_commands; i++)
    {
        delete commands[i];
    }
}
bool Compiler::cmp(char* str)                                     // compare extension with criticall extension for compiler
{
    Command* error = new Command(".pdf");
        if (error->equals(str))
        {
            return true;
        }
    return false;
}
