//============================================================================
// Name         : reader.cpp
// Author       : Ryan Elliot Hsu
// Date Created : 2021.11.26
// Date Modified: 2021.12.1
// Copyright    : All rights are reserved
// Description  : Assignment #3 reader
//============================================================================

#include<iostream>
#include<fstream>
#include<sstream>
#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/mman.h>
#include <chrono>
#include <thread>
#include"header.hpp"

using namespace std;
//============================================================================
int main (int argc, char *argv[])
{
    //assigning values to variables
    int lowerBound;
    int upperBound;
    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);
    int range = upperBound-lowerBound;

    //initiating and attaching shared memory segement
    int fd = shm_open("shm", O_RDWR, 00777);
    if (fd == -1)
    {
        perror("Cannot open shared memory file.");
        return 1;
    }
    struct shm* shareddata = (struct shm*)mmap(NULL, sizeof(struct shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shareddata == NULL)
    {
        perror("Cannot map shared memory.");
        return -1;
    }

    //operating semaphores
    sem_wait(shareddata->rc);
    shareddata->readerCounter++;
    if(shareddata->readerCounter==1)
    {
        sem_wait(shareddata->fileAccess);
    }
    sem_post(shareddata->rc);

    //opening the csv file to read
    fstream fin;
    fin.open("data.csv",ios::in);
    string line, word;

    //use getline() repeatedly until we reach the lower bound
    for (int i = 0; i < lowerBound-1; i++)
    {
        getline(fin, line);
    }
    
    for (int i = 0; i <= range; i++)
    {
        chrono::milliseconds timespan(100);
        this_thread::sleep_for(timespan);
        getline(fin, line);
        stringstream s(line);

        //printing out the record of student
        getline(s, word, ',');
        cout<<"Studnet ID: "<<word<<" | ";
        getline(s, word, ',');
        cout<<"Studnet Name: "<<word<<" | ";
        cout<<"Course Grades: ";
        getline(s, word, ',');
        cout<<word;
        for(int i=0; i<11; i++)
        {
            getline(s, word, ',');
            cout<<", "<<word;
        }
        getline(s, word, ',');
        cout<<" | GPA: "<<word<<endl;
    }
    fin.close();
    cout<<"\nReader finished."<<endl;

    //operating semaphores
    sem_wait(shareddata->rc);
    shareddata->readerCounter--;
    shareddata->readersProcessed++;
    cout<<"Number of readers processed: "<<shareddata->readersProcessed<<"\n"<<endl;
    if(shareddata->readerCounter==0)
    {
        sem_post(shareddata->fileAccess);
    }
    sem_post(shareddata->rc);
}
