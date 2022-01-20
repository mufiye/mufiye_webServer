# update in 2022.1.20
# try to use variable

targets = subTry1 subTry2 subTry3 subTry4
subTry1_file = subTry1.cpp
subTry2_file = subTry2.cpp
subTry3_file = subTry3.cpp
subTry4_file = ./subTry4_http/subTry4.cpp

all : $(targets)
subTry1 : $(subTry1_file)
	g++ -pthread -o subTry1 $(subTry1_file)
subTry2 : $(subTry2_file)
	g++ -g -pthread -o subTry2 $(subTry2_file)
subTry3 : $(subTry3_file)
	g++ -g -pthread -o subTry3 $(subTry3_file)
subTry4 : $(subTry4_file)
	g++ -g -pthread -o subTry4 $(subTry4_file)
clean : 
	rm $(targets)