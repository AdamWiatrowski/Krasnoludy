// main.cpp
#include <iostream>
#include <mpi.h>
#include "Skansen.h"
#include "Krasnal.h"

#include <cstdlib>
#include <ctime>

int main(int argc, char **argv)
{
    // Inicjalizacja MPI
    MPI_Init(&argc, &argv);
    
    // Zdefiniowanie numeru procesu (rank)
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Zdefiniowanie ilości procesów
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)) + rank);

    // Proces o rank 0 jest Root, pozostałe są Krasnalami
    if(rank == 0) {
        Skansen skansen(rank, size);
        skansen.introduce();
        skansen.generateOrder();
    }
    else {
        Krasnal krasnal(rank, size);
        krasnal.introduce();
        krasnal.mainActivity();
    }
    
    // Zakończenie MPI
    MPI_Finalize();

    return 0;
}

