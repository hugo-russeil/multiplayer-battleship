CC=gcc
CFLAGS=-O3

SERVER_SRC=$(wildcard server/src/*.c)
SERVER_INC=-Iserver/include
SERVER_OBJ=$(SERVER_SRC:.c=.o)

CLIENT_SRC=$(wildcard client/src/*.c)
CLIENT_INC=-Iclient/include
CLIENT_OBJ=$(CLIENT_SRC:.c=.o)

client: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_INC) $^ -o client.out

server: $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_INC) $^ -o server.out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	echo "Cleaning..."
	rm -f **/*.o client.out server.out

all: client server