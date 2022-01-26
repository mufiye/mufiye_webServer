acceptor_h = ./acceptor/acceptor.h
acceptor_cpp = ./acceptor/acceptor.cpp
rw_buffer_h = ./buffer/rw_buffer.h
rw_buffer_cpp = ./buffer/rw_buffer.cpp
common_h = ./common/common.h
common_cpp = ./common/common.cpp
simple_connection_h = ./connection/simple_connection.h
simple_connection_cpp = ./connection/simple_connection.cpp
tcp_connection_h = ./connection/tcp_connection.h
tcp_connection_cpp = ./connection/tcp_connection.cpp
locker_h = ./locker/locker.h
locker_cpp = ./locker/locker.cpp
thread_pool_h = ./threadPool/threadPool.h
objects = acceptor.o rw_buffer.o common.o tcp_connection.o simple_connection.o locker.o main.o
headers = $(acceptor_h) $(rw_buffer_h) $(common_h) $(simple_connection_h) \
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
simple_connection.o : $(simple_connection_cpp) $(simple_connection_h) $(tcp_connection_h) $(rw_buffer_h)
	g++ -g -c $(simple_connection_cpp)
tcp_connection.o : $(tcp_connection_cpp) $(tcp_connection_h) $(rw_buffer_h)
	g++ -g -c $(tcp_connection_cpp)
locker.o : $(locker_cpp) $(locker_h) $(common_h)
	g++ -g -pthread -c $(locker_cpp)
main.o : main.cpp $(acceptor_h) $(rw_buffer_h) $(thread_pool_h) $(simple_connection_h)
	g++ -g -pthread -c main.cpp
clean :
	rm *.o server