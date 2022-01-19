all : client
client : clientTest.o
	g++ -pthread -o client clientTest.o
clientTest.o : clientTest.cpp
	g++ -c clientTest.cpp
clean :
	rm *.o client