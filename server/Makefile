CC=g++
CFLAGS= -pthread
#=========================================================
#add any new header files to DEPS macro and any new object
#files to OBJ macro

DEPS = http_get_request_parser.h http_response_builder.h \
	   server_socket.h user.h client_socket.h server_manager.h

OBJ = http_get_request_parser.o http_response_builder.o \
	  server_actions.o server.o server_socket.o user.o \
	  server_manager.o
#=========================================================

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all : server

server : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *o *gch server
