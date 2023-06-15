#include "Skansen.h"

Skansen::Skansen(int id, int size) : m_id(id), m_size(size) {}

void Skansen::introduce()
{
    std::cout << "[" << m_id << "] Jestem skansen.\n";
    sleep(2);
}

void Skansen::generateOrder() {
    while (true) {
        std::cout << "[" << m_id << "] Wygenerowalem zlecenie. " << "\n";
        sleep(rand() % 5 + 1);
        
        for (int dwarfId = 1; dwarfId < m_size; dwarfId++) {
            MPI_Send(NULL, 0, MPI_INT, dwarfId, JOB_REQUEST, MPI_COMM_WORLD);
        }
        
        waitForRelease();
    }
}

void Skansen::waitForRelease() {
    MPI_Status status;
    int recv_ID;
    MPI_Recv(&recv_ID, 1, MPI_INT, MPI_ANY_SOURCE, RELEASE, MPI_COMM_WORLD, &status);
    std::cout << "[" << m_id << "] Orzymalem REL od: " << recv_ID << ".\n";
    sleep(2);
}