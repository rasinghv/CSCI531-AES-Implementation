CC = g++
FLAGS = -Wall -g
OUTPUT = hw6

hw6: utilities.o tablecheck.o modprod.o encrypt.o hw6.cpp
	$(CC) $(FLAGS) utilities.o tablecheck.o modprod.o encrypt.o hw6.cpp -o $(OUTPUT)

utilities.o: utilities.cpp utilities.h
	$(CC) $(FLAGS) -c utilities.cpp

tablecheck.o: tablecheck.cpp tablecheck.h
	$(CC) $(FLAGS) -c tablecheck.cpp

modprod.o: modprod.cpp modprod.h
	$(CC) $(FLAGS) -c modprod.cpp

encrypt.o: encrypt.cpp encrypt.h
	$(CC) $(FLAGS) -c encrypt.cpp

clean:
	rm -f *.o $(OUTPUT)
