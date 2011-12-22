CC=g++
CFLAGS= -pthread
#=========================================================
#add any new header files to DEPS macro and any new object
#files to OBJ macro

DEPS = http_get_request_parser.h http_response_builder.h \
	   server_socket.h user.h client_socket.h server_manager.h \
	   ftp_server.h command_support.h ftp_command_parser.h \
	   server_constants.h bernoulli_trial.h packet.h \
	   mutex_timer.h

OBJ = http_get_request_parser.o http_response_builder.o \
	  server_actions.o server.o server_socket.o user.o \
	  server_manager.o ftp_server.o commands_support.o \
	  ftp_command_parser.o bernoulli_trial.o packet.o \
	  mutex_timer.o
#=========================================================

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all : server

server : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *o *gch server
