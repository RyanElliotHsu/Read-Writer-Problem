//============================================================================
// Name         : writer.cpp
// Author       : Ryan Elliot Hsu
// Date Created : 2021.11.26
// Date Modified: 2021.12.1
// Copyright    : All rights are reserved
// Description  : Assignment #3 writer
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
#include<iomanip>
#include <chrono>
#include <thread>
#include"header.hpp"
#include<sys/mman.h>

using namespace std;
//============================================================================
int main (int argc , char *argv [])
{
    //assigning values to variables
    int lowerBound;
    int upperBound;
    int numRecords = 100;
    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);
    int range = upperBound-lowerBound;

    //initiating and attaching shared memory segement
    int fd = shm_open("shm", O_RDWR, 00777);
    if (fd == -1)
    {
        perror("Cannot open shared memory file");
        return 1;
    }
    struct shm* shareddata = (struct shm*)mmap(NULL, sizeof(struct shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shareddata == NULL)
    {
        perror("Cannot map shared memory");
        return -1;
    }

    //operating semaphores
    sem_wait(shareddata->fileAccess);
    cout<<"\nWriter starting..."<<endl;
    chrono::milliseconds timespan(1000);
    this_thread::sleep_for(timespan);

    //opening file to read data from
    fstream fin;
    string line, word;
    fin.open("data.csv",ios::in);
    
    //open new file to write updated data
    ofstream newFile("newfile.csv");

    //use getline() to iterate through records until lower bound
    //and append to new file
    for (int i = 1; i < lowerBound; i++)
    {
        getline(fin, line);
        newFile<<line;
    }

    //change course grades for student records within the given boundaries
    for (int i = lowerBound; i <= upperBound; i++)
    {
        //random number from 1 to 100
        int changeOrNot = rand()%100+1;
        //75% of the time, the student record is not changed and simply copied to the new file
        if (changeOrNot > 25)
        {
            getline(fin, line);
            newFile<<line;
        }

        //in the remaining 25%, the grades are changed randomly then added to the new file
        else
        {
            string newline;
            getline(fin, line);
            stringstream s(line);
            getline(s, word, ',');
            newline += word += ",";
            getline(s, word, ',');
            newline += word += ",";

            //assign random new grades, divided by 2 to create grades 0.5, 1.5, etc
            float gradeSum = 0;
            for (int j = 0; j < 12; j++)
            {
                float grade = (rand()%8)/float(2);
                gradeSum += grade;
                string gradeString = to_string(grade);
                string gradeSubString = gradeString.substr(0,3);
                newline += gradeSubString += ",";
            }

            //calculating gpa and adding the new record line into the new file
            float gpa = gradeSum/12;
            string gpaString = to_string(gpa);
            string gpaSubString = gpaString.substr(0,4);
            newline += gpaSubString;
            newFile<<newline<<'\n';
        }
    }

    //use getline() to iterate through records from upperbound
    //to end and append to new file
    for (int i = upperBound; i < numRecords; i++)
    {
        getline(fin, line);
        newFile<<line;
    }

    fin.close();
    newFile.close();
    cout<<"Writer finished, file updated successfully."<<endl;
    shareddata->writersProcessed++;
    cout<<"Number of writers processed: "<<shareddata->writersProcessed<<"\n"<<endl;

    //replace the old file with the new file
    //remove("data.csv");
    //rename("newfile.csv", "data.csv");

    //operating semaphores
    sem_post(shareddata->fileAccess);
}
