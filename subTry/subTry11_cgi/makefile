all : subTry11
subTry11 : subTry11.o ../subTry10_acceptor/acceptor.o ../subTry10_acceptor/acceptor.h
	g++ -o subTry11 subTry11.o ../subTry10_acceptor/acceptor.o
subTry11.o : subTry11.cpp ../subTry10_acceptor/acceptor.h
	g++ -c subTry11.cpp
clean :
	rm *.o subTry11