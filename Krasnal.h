// Krasnal.h
#ifndef KRASNAL_H
#define KRASNAL_H

#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>


#define REQUEST 1
#define REPLY 2
#define RELEASE 3

#define JOB_REQUEST 5

#define skansen_id 0

struct Data
{
    int ID;
    int clock;
};

class Krasnal
{
public:

    Krasnal(int id, int size);

    void introduce();
    void mainActivity();

    Data m_data;

private:

    int m_id;
    int m_size;

    void sleepyDwarf();
    void requestJob();
    void receiveJob();
    void waitForReplies();
    void waitForTurn();
    void doJob();
    void sendRelease();

    void updateClock();
    void compareClock(const Data& data);

    // kolejka
    std::vector<Data> queue;

    void addToQueue(const Data& data);
    void sortQueue();
    void removeAndSortQueue(int id);

    // muteksy
    std::mutex clockMutex;
    std::mutex queueMutex;
    std::mutex relMutex;

    // watki
    std::thread reqThread;
    std::thread relThread;

    void reqFunction();
    void relFunction();


};

#endif // KRASNAL_H
