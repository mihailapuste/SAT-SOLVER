all: lab5

lab5: lab5.o
	g++ -o lab5 lab5.o

lab5.o: lab5.cpp 
	g++ -c lab5.cpp

clean:
	rm -f lab5 *.o
