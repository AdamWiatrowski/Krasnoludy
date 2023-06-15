# Zmienna przechowująca kompilator MPI
MPICXX = mpic++

# Opcje kompilacji
CXXFLAGS = -std=c++17

# Pliki źródłowe
SRC = Skansen.cpp Krasnal.cpp main.cpp

# Plik wykonywalny
TARGET = job

all: $(TARGET)

$(TARGET): $(SRC)
	$(MPICXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
