all : subTry10
subTry10 : subTry10.o acceptor.o acceptor.h
	g++ -o subTry10 acceptor.o subTry10.o
subTry10.o : subTry10.cpp acceptor.h
	g++ -c subTry10.cpp
acceptor.o : acceptor.cpp acceptor.h
	g++ -c acceptor.cpp
clean : 
	rm *.o subTry10