all : subTry1 subTry2
subTry1 : subTry1.cpp
	g++ -pthread -o subTry1 subTry1.cpp
subTry2 : subTry2.cpp
	g++ -g -pthread -o subTry2 subTry2.cpp
subTry3 : subTry3.cpp
	g++ -g -pthread -o subTry3 subTry3.cpp
clean : 
	rm subTry1 subTry2 subTry3