CC		:= gcc
CFLAGS		+= -D=_GNU_SOURCE -D=TCP -pthread -Wall -Werror #-pedantic 
LDFLAGS 	= -lpthread


OBJECTS		:= main.o server.o client.o common.o

main: $(OBJECTS)

clean:
	@rm -f $(OBJECTS) main
