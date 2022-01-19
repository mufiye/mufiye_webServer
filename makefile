all : server
server : main.o
	g++ -o server main.o
main.o : main.cpp
	g++ -c main.cpp
clean :
	rm *.o server