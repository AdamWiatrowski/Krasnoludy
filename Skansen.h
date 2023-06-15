// Skansen.h
#ifndef SKANSEN_H
#define SKANSEN_H

#include <iostream>
#include <mpi.h>
#include <unistd.h>

#define REQUEST 1
#define REPLY 2
#define RELEASE 3

#define JOB_REQUEST 5

class Skansen
{
public:
    Skansen(int id, int size);
    void introduce();
    void generateOrder();
private:
    int m_id;
    int m_size;
    void waitForRelease();
};

#endif // SKANSEN_H
