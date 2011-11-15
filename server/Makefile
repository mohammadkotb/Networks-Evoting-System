CC=g++
CFLAGS= -pthread
#=========================================================
#add any new header files to DEPS macro and any new object
#files to OBJ macro

DEPS = http_get_request_parser.h http_response_builder.h \
	   server_socket.h

OBJ = http_get_request_parser.o http_response_builder.o \
	  server_actions.o web_server.o server_socket.o
#=========================================================

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all : web_server

web_server : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *o *gch web_server
