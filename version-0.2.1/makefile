acceptor_h = ./acceptor/acceptor.h
acceptor_cpp = ./acceptor/acceptor.cpp
rw_buffer_h = ./buffer/rw_buffer.h
rw_buffer_cpp = ./buffer/rw_buffer.cpp
common_h = ./common/common.h
common_cpp = ./common/common.cpp
http_connection_h = ./connection/http_connection.h
http_connection_cpp = ./connection/http_connection.cpp
tcp_connection_h = ./connection/tcp_connection.h
tcp_connection_cpp = ./connection/tcp_connection.cpp
locker_h = ./locker/locker.h
locker_cpp = ./locker/locker.cpp
thread_pool_h = ./threadPool/threadPool.h
objects = acceptor.o rw_buffer.o common.o tcp_connection.o http_connection.o locker.o main.o
headers = $(acceptor_h) $(rw_buffer_h) $(common_h) $(http_connection_h) \
		  $(tcp_connection_h) $(locker_h) $(thread_pool_h)

all : server
server : $(objects) $(headers)
	g++ -g -pthread -o server $(objects)
acceptor.o : $(acceptor_cpp) $(acceptor_h) $(common_h)
	g++ -g -c $(acceptor_cpp)
rw_buffer.o : $(rw_buffer_cpp) $(rw_buffer_h) $(common_h)
	g++ -g -c $(rw_buffer_cpp)
common.o : $(common_h) $(common_cpp)
	g++ -g -c $(common_cpp)
http_connection.o : $(http_connection_cpp) $(http_connection_h) $(tcp_connection_h) $(rw_buffer_h) $(common_h)
	g++ -g -c $(http_connection_cpp)
tcp_connection.o : $(tcp_connection_cpp) $(tcp_connection_h) $(rw_buffer_h) $(common_h)
	g++ -g -c $(tcp_connection_cpp)
locker.o : $(locker_cpp) $(locker_h) $(common_h)
	g++ -g -pthread -c $(locker_cpp)
main.o : main.cpp $(acceptor_h) $(rw_buffer_h) $(thread_pool_h) $(http_connection_h) $(tcp_connection_h) $(common_h)
	g++ -g -pthread -c main.cpp
clean :
	rm *.o server