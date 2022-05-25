EXECUTABLE=voto.exe

compilar: ./src/*.cpp ./src/*.hpp # Compilar código c++
	g++ -o $(EXECUTABLE) ./src/*.cpp ./src/*.hpp -std=c++17 

.PHONY: lint run clean

lint:
	cpplint ./src/main.cpp ./src/FS.hpp

run: compilar
	.\$(EXECUTABLE)

clean:
	del $(EXECUTABLE)

