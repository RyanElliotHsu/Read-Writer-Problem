//============================================================================
// Name         : header.cpp
// Author       : Ryan Elliot Hsu
// Date Created : 2021.11.26
// Date Modified: 2021.12.1
// Copyright    : All rights are reserved
// Description  : Assignment #3 header
//============================================================================
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
using namespace std;

struct Node
{
    int pid;
    Node* next;
    Node(int p){pid=p; next=NULL;} 
};

struct Queue
{
    Node *first, *last;
    Queue(){first = last = NULL;}

    void push(int x)
    {
        Node* temp = new Node(x);
        if(first==NULL)
        {
            first = last = temp;
            return;
        }

        last->next = temp;
        last = temp;
    }

    int pop()
    {
        if(first==NULL)
        {
            return 0;
        }

        int front = first->pid;
        Node* temp = first;
        first = first->next;

        if(first==NULL)
        {
            last = NULL; //if the first node is null, then the queue is empty
        }

        delete(temp);
        return front;
    }

};

struct shm
{
    int readerCounter;
    int writerCounter;
    int readersProcessed;
    int writersProcessed;
    int readersDuration;
    int writersDuration;
    int maxDelay;
    int recordsReached;
    Queue *q;
    sem_t* fileAccess;
    sem_t* rc;
};