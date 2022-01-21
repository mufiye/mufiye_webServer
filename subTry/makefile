# update in 2022.1.20
# try to use variable

targets = subTry1 subTry2 subTry3 subTry5
subTry1_file = subTry1.cpp
subTry2_file = subTry2.cpp
subTry3_file = subTry3.cpp
subTry5_dir = ./subTry5_http
subTry5_file = ./subTry5_http/subTry5.cpp

all : $(targets)
subTry1 : $(subTry1_file)
	g++ -pthread -o subTry1 $(subTry1_file)
subTry2 : $(subTry2_file)
	g++ -g -pthread -o subTry2 $(subTry2_file)
subTry3 : $(subTry3_file)
	g++ -g -pthread -o subTry3 $(subTry3_file)
subTry5 : $(subTry5_file) http_response.o $(subTry5_dir)/http_response.h
	g++ -g -o subTry5 $(subTry5_file) http_response.o
http_response.o : $(subTry5_dir)/http_response.cpp $(subTry5_dir)/http_response.h
	g++ -c $(subTry5_dir)/http_response.cpp 
clean : 
	rm *.o $(targets)