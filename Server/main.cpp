#ifdef WIN32
#define CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <conio.h>
#include <process.h>
#include <locale.h>
#include <fstream>
#include <stdlib.h>
#endif
#ifdef linux
#include <QCoreApplication>
#include "compiler.h"
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
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define PIPE_NAME "/home/alexey/Lab_1/fifofile"
#define SEMAPHORE_NAME "/my_named_semaphore"


#endif

using namespace std;

#define BUFFSIZE 256
#define CLIENT_COUNT 3                                               // the count of the clients


int main(int argc, char *argv[])

#ifdef WIN32
{
    BOOL fConnected;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    int flag;

    HANDLE hNamedPipe;


    while (1) {

        LPTSTR PipeName = TEXT("\\\\.\\pipe\\TopPipeInDaWorld");

        hNamedPipe = CreateNamedPipe(
                    PipeName,
                    PIPE_ACCESS_DUPLEX,
                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                    PIPE_UNLIMITED_INSTANCES,
                    512, 512, NMPWAIT_USE_DEFAULT_WAIT, NULL);


        CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab1_ProcessesAndPipes\\ChildProcess\\Debug\\ChildProcess.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

        ConnectNamedPipe(hNamedPipe, NULL);

        DWORD NumBytesToWrite;
        DWORD dWait = -1;
        string str;
        system("cls");
        cout << "Enter program you want to built: " << endl;
        getline(cin, str);
        WriteFile(hNamedPipe, str.c_str(), str.length(), &NumBytesToWrite, NULL);

        while (dWait != WAIT_OBJECT_0)
        {
            system("cls");
            cout << "Waiting..." << endl;
            dWait = WaitForSingleObject(pi.hProcess, INFINITE);
        }
        system("cls");

        DWORD iNumBytesToRead;
        char   Buff[BUFFSIZE];

        ReadFile(hNamedPipe, Buff, 512, &iNumBytesToRead, NULL);

        int i;
        for (i = 0; i < iNumBytesToRead; i++)
        {
            cout << Buff[i];
        }
        printf("\n");
        printf("Do you want to continue?\n");
        cin >> flag;
        cin.ignore(1, '\n');
        if (flag != 1)
            break;
    }
#endif

#ifdef linux
    {

        //-----------------------MANIPULATOR PROCESS-------------------------------

        int signo;
        sigset_t newmask;                                                // mask for signals
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newmask, NULL);

        unlink(PIPE_NAME);
        mkfifo(PIPE_NAME, O_RDWR);                                       // сreating pipe for communicating between clients and server
        unlink(PIPE_NAME);
        sem_t *mutex;                                                    // declare semaphore
        int value = 0;
        mutex = sem_open(SEMAPHORE_NAME,O_CREAT,0644,0);                 // creating semaphore (mutex)
        if(mutex == SEM_FAILED)
        {
            perror("unable to create semaphore");
            sem_unlink(SEMAPHORE_NAME);                                  // destroying semaphore
            return -1;
        }
        sem_post(mutex);                                                 // release semaphore
        sem_getvalue(mutex,&value);
        printf("semaphore - %d\n",value);
        char buf[100];
        for(int i = 0; i < CLIENT_COUNT; i++)
        {
            cout << "Client process has been created" << endl;
            strcpy(buf,"gnome-terminal -x sh -c 'exec /home/alexey/Lab_1/build-Client-Desktop_Qt_5_8_0_GCC_64bit-Debug/Client ");
            strcat(buf,std::to_string((int)getpid()).c_str());           // add pid to command argument
            strcat(buf,"'");
            system(buf);                                                 // create client
            printf("%d\n",i);
            printf("Server - %d\n", (int)getpid());                      // server pid
        }

        //-------------------SERVER PROCESS------------------------------------

        printf("after create\n");
        Compiler *compiler = new Compiler(6);                            // 7 commands
        int clientClose = CLIENT_COUNT;
        while (clientClose > 0)                                          // while not all was closed
        {
            printf("WAITING\n");
            sigwait(&newmask,&signo);                                    // wait signal
            printf("get signal\n");
            if (signo == SIGUSR1)
            {
                clientClose--;
                printf("GET SIGNAL\n");
                if(!compiler->connectToPipe(PIPE_NAME)){                 // try to connect to pipe
                    printf("compiler can't connect\n");
                    return -1;
                }
                char* str = new char[BUFFSIZE];
                switch( compiler->run(str) )                             // read and compare string
                {
                case 1:
                {
                    printf("YES\n");                                     // if progam is supported
                    break;
                }
                case -1:
                {
                    printf("PDF ERROR\n");                               // if PDF format
                    break;
                }
                case 0:
                {
                    printf("NO\n");                                      // if No supported
                    break;
                }
                }
                compiler->closeConnectionToPipe();                       // close connection
                sem_post(mutex);                                         // release semaphore
            }
        }
        sem_close(mutex);                                                // close semaphore
        sem_destroy(mutex);                                              // destroy semaphore
        unlink(PIPE_NAME);                                               // close Pipe
        return 0;
    }
#endif

