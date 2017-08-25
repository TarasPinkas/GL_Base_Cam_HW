CC		:= gcc
CFLAGS		+= -pthread -Wall -pedantic 
LDFLAGS 	= -lpthread

OBJECTS		:= main.o server.o client.o 

main: $(OBJECTS)

clean:
	@rm -f $(OBJECTS) main
