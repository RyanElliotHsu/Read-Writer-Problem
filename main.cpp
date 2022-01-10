//============================================================================
// Name         : main.cpp
// Author       : Ryan Elliot Hsu
// Date Created : 2021.11.26
// Date Modified: 2021.12.1
// Copyright    : All rights are reserved
// Description  : Assignment #3 main
//============================================================================

#include<iostream>
#include<sstream>
#include<cmath>
#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include"header.hpp"

using namespace std;
//============================================================================
int main(int argc, char *argv[])
{
    int numProcesses;
    int readerWorkTime;
    int writerWorkTime;
    srand((unsigned)time(NULL));
    /*numProcesses = atoi(argv[0]);
    readerWorkTime = atoi(argv[1]);
    writerWorkTime = atoi(argv[2]);*/

    numProcesses = 10;

    //initiating and creating shared memory segment
    int fd = shm_open("shm", O_RDWR | O_CREAT, 00777);
    if (fd == -1)
    {
        perror("Cannot open shared memory file");
        return 1;
    }
    int resize = ftruncate(fd, sizeof(struct shm));
    struct shm* shareddata = (struct shm*)mmap(NULL, sizeof(struct shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shareddata == NULL)
    {
        perror("Cannot map shared memory");
        return -1;
    }

    //initializing all values in the shared memory to 0
    shareddata->maxDelay = 0;
    shareddata->readerCounter = 0;
    shareddata->readersDuration = 0;
    shareddata->readersProcessed = 0;
    shareddata->recordsReached = 0;
    shareddata->writerCounter = 0;
    shareddata->writersDuration = 0;
    shareddata->writersProcessed = 0;
    shareddata->q = new Queue();

    //unlink semaphores to avoid previously set values
    sem_unlink("fileaccess");
    sem_unlink("readerCount");

    //initialize semaphores
    shareddata->fileAccess = sem_open("fileaccess", O_CREAT,  0666, 1);
    shareddata->rc = sem_open("readerCount", O_CREAT,  0666, 1);

    int numRecords = 100;
    int lowerBound;
    int upperBound;

    for(int i=0; i<numProcesses; i++)
    {
        int child = fork();

        if(child==0)
        {
            //randoming assign values for lower and upper bounds
            int pid = getpid(); //use pid for seed as randomness for readers/writers
            lowerBound = rand()%(numRecords-2);
            int range = rand()%(numRecords-lowerBound)+1;   //upperbound must be higher than lowerbound, cannot directly assign upperbound
            upperBound = lowerBound+range;

            //converting int to char* so exec can take the parameter
            string s = to_string(lowerBound);
            char const* lb_const = s.c_str();
            char* lb = (char*)lb_const;
            string s2 = to_string(upperBound);
            char const* ub_const = s2.c_str();
            char* ub = (char*)ub_const;

            //readerOrWriter = 2;

            // 2/3 probability that a process will be reader
            if (pid%3==0 || pid%3==1)
            {
                char* args[] = {"./reader",lb, ub, NULL};
                execvp(args[0],args);
            }

            // 1/3 probability that a process will be writer
            else if (pid%3==2)
            {
                char* args[] = {"./writer", lb, ub, NULL};
                execvp(args[0],args);
            }
            exit(0);
        }

        else if (child>0)
        {
            wait(NULL);
        }
    }

    return 0;
}