// Krasnal.cpp
#include "Krasnal.h"

Krasnal::Krasnal(int id, int size) : m_id(id), m_size(size)
{
    m_data.ID = m_id;
    m_data.clock = 0;

    reqThread = std::thread(&Krasnal::reqFunction, this);
    relThread = std::thread(&Krasnal::relFunction, this);
}

void Krasnal::introduce()
{
    std::cout << "[" << m_id << "] Jestem krasnal.\n";
    sleep(2);
}


void Krasnal::sleepyDwarf() {
    std::cout << "[" << m_id << "] Odpoczywam.\n";
    int sleepDuration = (std::rand() % 3) + 2;

    sleep(sleepDuration);
}

void Krasnal::updateClock()
{
    std::cout << "[" << m_id << "] Zwiekszam zegar.\n";

    clockMutex.lock();

    m_data.clock++;

    clockMutex.unlock();

}

void Krasnal::compareClock(const Data& data)
{
    std::cout << "[" << m_id << "] Porownuje zegar.\n";

    clockMutex.lock();

    m_data.clock = std::max(m_data.clock, data.clock) + 1;

    clockMutex.unlock();

}

void Krasnal::sortQueue() {
    
    queueMutex.lock();
    std::sort(queue.begin(), queue.end(), [](const Data& a, const Data& b) {
        if (a.clock == b.clock) {
            return a.ID < b.ID;
        }
        return a.clock < b.clock;
    });
}

void Krasnal::removeAndSortQueue(int id) {

    std::cout << "[" << m_id << "] Usuwam z kolejki "<< id <<".\n";
    
    queueMutex.lock();

    auto it = std::find_if(queue.begin(), queue.end(), [id](const Data& data) {
        return data.ID == id;
    });

    if (it != queue.end()) {
        queue.erase(it);
        queueMutex.unlock();
        sortQueue();
        queueMutex.unlock();
    }

    
}

void Krasnal::addToQueue(const Data& data) {

    std::cout << "[" << m_id << "] Dodaje do kolejki " << data.ID << ".\n";

    queueMutex.lock();
    queue.emplace_back(data);
    queueMutex.unlock();

    sortQueue();
    queueMutex.unlock();
}

void Krasnal::requestJob() {

    std::cout << "[" << m_id << "] Ubiegam sie o prace.\n";

    int data[2];

    data[0] = m_data.ID;
    data[1] = m_data.clock;


    for (int i = 1; i < m_size; i++) {
        if (i != m_id) {
            MPI_Send(&data, 2, MPI_INT, i, REQUEST, MPI_COMM_WORLD);
        }
    }
}

void Krasnal::waitForReplies(){

    std::cout << "[" << m_id << "] Oczekuje na odpowiedzi.\n";

    MPI_Status status;
    int success;

    for (int i = 1; i < m_size; i++) {
        if (i != m_id) {
            MPI_Recv(&success, 1, MPI_INT, i, REPLY, MPI_COMM_WORLD, &status);
        }
    }
    std::cout << "[" << m_id << "] Otrzymalem odpowiedzi.\n";
}

void Krasnal::receiveJob()
{
    MPI_Status status;

    MPI_Recv(NULL, 0, MPI_INT, skansen_id, JOB_REQUEST, MPI_COMM_WORLD, &status);
    std::cout << "[" << m_id << "] Otrzymalem prace od Skansenu.\n";
}

void Krasnal::waitForTurn() {
    std::cout << "[" << m_id << "] Czekam na prace.\n";
    while (true) {
        queueMutex.lock();
        std::cout << queue.front().ID << "\n";
        if(!queue.empty() && queue.front().ID == m_id) {
            queueMutex.unlock();
            break;
        } 
        queueMutex.unlock();
        sleep(1);
    }
}

void Krasnal::doJob() {
    std::cout << "[" << m_id << "] Wykonuje prace.\n";

    int jobDuration = (std::rand() % 3) + 2;

    sleep(jobDuration);
}

void Krasnal::sendRelease() {

    std::cout << "[" << m_id << "] Wysylam REL.\n";

    for (int i = 0; i < m_size; i++) {
        if (i != m_id) {
            MPI_Send(&m_data.ID, 1, MPI_INT, i, RELEASE, MPI_COMM_WORLD);
        }
    }
}



void Krasnal::relFunction()
{
    int recv_ID;
    MPI_Status status;

    while (true)
    {
        
        MPI_Recv(&recv_ID, 1, MPI_INT, MPI_ANY_SOURCE, RELEASE, MPI_COMM_WORLD, &status);

        std::cout << "[" << m_id << "] Otrzymalem REL od" << recv_ID << ".\n";

        updateClock();
        removeAndSortQueue(recv_ID);

        queueMutex.unlock();

        sleep(1);
    }
}

void Krasnal::reqFunction()
{
    int success = 1;
    int data[2];
    MPI_Status status;

    while (true)
    {
        //std::cout << "[" << m_id << "] REQ\n";
        MPI_Recv(&data, 2, MPI_INT, MPI_ANY_SOURCE, REQUEST, MPI_COMM_WORLD, &status);

        std::cout << "[" << m_id << "] Otrzymalem REQ od " << data[0] << ".\n";

        Data recv_data{data[0], data[1]};

        addToQueue(recv_data);
        compareClock(recv_data);

        std::cout << "[" << m_id << "] Wysylam. \n";

        MPI_Send(&success, 1, MPI_INT, recv_data.ID, REPLY, MPI_COMM_WORLD);
        
        sleep(1);
    }
}

void Krasnal::mainActivity(){
    while (true)
    {
        sleepyDwarf();
        updateClock();

        requestJob();

        Data tempData{m_data.ID, m_data.clock};
        addToQueue(tempData);

        waitForReplies();

        waitForTurn();

        receiveJob();

        doJob();

        sendRelease();

        removeAndSortQueue(m_data.ID);

    }
    
}
