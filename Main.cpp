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
#include "programmer.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* read string from file */
void read_from_pipe(int file, char *str)                  //read from pipe
{
    int len = 0;
    char c;
    do
    {
        read(file, &c, 1);                                //read one char from pipe
        str[len] = c;
        len++;
    } while (c != '\0'&& c != EOF);                       //while not end of string
    (str)[len] = '\0';
}

/* clean stdio (handmade rewind) */
void clean_stdin(void)                                    // clean stdin
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}


/* Write some text to the pipe. */

void write_to_pipe(int file, char* msg)                    //write messege in pipe
{
    write(file, msg, strlen(msg) + 1);
    return;
}

#endif 

using namespace std;
#define BUFFSIZE 256



int main(int argc, char *argv[])
{

	#ifdef WIN32
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

    QCoreApplication a(argc, argv);
		printf("run\n");
        pid_t pid;                                         // id of child process
                                                           // int station;
        int mypipe[2];                                     // creating pipes

															/* Create the pipe. */
		if (pipe(mypipe))                                  // if creting pipes
		{
			fprintf(stderr, "Pipe failed.\n");
			return -1;
		}

        switch (pid = fork())                              // create process
		{
		case -1:
		{
			perror("fork");
			exit(1);
		}
		case 0:
		{
			// ----------------CHILD PROCESS---------------------------
            int signo;                                     // set the mask for getting signals
			sigset_t newmask;
            sigemptyset(&newmask);
            sigaddset(&newmask, SIGUSR1);                  // add signals in mask
            sigaddset(&newmask, SIGUSR2);
            sigaddset(&newmask, SIGINT);
            sigprocmask(SIG_BLOCK, &newmask, NULL);


            char choice = '1';                             // choice of continue
            char *str = new char[BUFFSIZE];
			while (choice == '1')
			{
                close(mypipe[0]);                          // close 0 end
                printf("\nEnter programm you want to build:\n");
                gets(str);                                 // get inquery
                write_to_pipe(mypipe[1], str);             // write in pipe
                kill(getppid(), SIGUSR1);                  // make signal for parent process
                sigwait(&newmask, &signo);                 // wait for signal
                if (signo == SIGUSR1)                      // 1 - signal "YES"
				{
                    printf("\nget answer - Program opened...\n");
				}
				else
                {
                    if (signo == SIGUSR2)                  // 2 - signal "NO"
                    {
                        printf("\nget answer - Not supported...\n");
                    }
                    else
                    {
                        printf("answer - Can't open this format..\n"); // other - signal "ERROR"
                    }
				}
                printf("\ninput 1 for continue\n");        // continue process
                scanf("%c", &choice);                      // scanf choice
                clean_stdin();                             // clear the stdin
			}
            kill(getppid(), SIGUSR2);                      // signal of quit from program
			return 0;
		}
		default:
			//---------------PARENT PROCESS------------------------
		{
            Compiler* res = new Compiler(7);               // 7 commands
												 /* This is the parent process.
                                                 Close other end first. */
            int signo;                                     // set mask for signals
			sigset_t newmask;
            sigemptyset(&newmask);
            sigaddset(&newmask, SIGUSR1);                  // add signals in the mask
			sigaddset(&newmask, SIGUSR2);
            sigprocmask(SIG_BLOCK, &newmask, NULL);        // block SIGUSR1
            char *str = new char[BUFFSIZE];                // string for read
			while (1)
			{
                sigwait(&newmask, &signo);                 // wait child process
				if (signo == SIGUSR1)
				{
                    close(mypipe[1]);                      // close 1 end of pipe
                    read_from_pipe(mypipe[0], str);        // read from pipe in buffer
                    printf("%s -- ", str);
                    if (res->getCommand(str))              // process result
					{
                        printf("YES\n");                   // if extension is supported
                        kill(pid, SIGUSR1);                // send 1 SIGNAL
					}
					else
                    {
                        if (res->cmp(str))
                        {
                            printf("ERROR\n");             // if extension is critical for compiler
                            kill(pid,SIGINT);              // send 3 SIGNAL
                        }
                        else
                        {
                            printf("NO\n");                // if extension not supported
                            kill(pid, SIGUSR2);            // send 2 SIGNAL
                        }
					}
				}
				else
				{
					break;
				}
			}
			return 0;
		}
		}
		return a.exec();
	}

#endif

