CC=g++
CFLAGS=-lsqlite3
all:client dbserver ioserver
client: client.cpp
		$(CC) $^ -o $@
dbserver: dbserver.cpp
		$(CC) $^ -o $@ $(CFLAGS)
ioserver: ioserver.cpp
		$(CC) $^ -o $@ $(CFLAGS)
clean:
		rm -r client dbserver ioserver my.db
