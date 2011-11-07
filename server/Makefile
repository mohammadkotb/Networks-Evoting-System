CC=g++
#=========================================================
#add any new header files to DEPS macro and any new object
#files to OBJ macro

DEPS = http_get_request_parser.h http_response_builder.h

OBJ = http_get_request_parser.o http_response_builder.o \
	  server_actions.o
#=========================================================

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $<

all : server

server : $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -rf *o *gch server
