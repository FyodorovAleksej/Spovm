#define CRT_SECURE_NO_WARNINGS
#include <QCoreApplication>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "programmer.h"


#define SEMAPHORE_NAME "/my_named_semaphore"                         // name of the semaphore
#define BUFFSIZE 256                                                 // bufsize
#define PIPE_NAME "/home/alexey/Lab_1/fifofile"                                       // Pipe name
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // ----------------------CLIENT PROCESS------------------------------------

    int parentID = 0;
    parentID = atoi(argv[1]);                                        // get ppid from a command line

    char str[BUFFSIZE];
    sem_t *mutex;                                                    // semaphore
    mutex = sem_open(SEMAPHORE_NAME,0,0644,0);

    printf("command parent - %d\n",parentID);
    if(mutex == SEM_FAILED)
    {
        perror("reader:unable to execute semaphore");
        sem_close(mutex);
        exit(-1);
    }

    printf("Wait...\n");    
    sem_wait(mutex);
    Programmer *programmer = new Programmer();                       // waiting semaphore
    printf("Input Program:\n");
    gets(str);
    printf("\ngood job\n");
    sleep(1);
    if(!programmer->connectToPipe(PIPE_NAME))                        // connect to pipe
    {
        printf("cannot connect\n");
        return -1;
    }
    printf("running compile\n");
    sleep(1);
    programmer->runCompile(str);                                     // write string in pipe
    programmer->closeConnectionToPipe();                             // close pipe
    printf("kill\n");
    kill((pid_t)parentID,SIGUSR1);                                   // send to parent process SIGUSR1
    sem_close(mutex);                                                // close semaphore
    sleep(1);                                                        // for reading messages
}

