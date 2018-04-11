CC = gcc
CFLAGS = -g -Wall -o1 -pedantic

nothing:

sender: sender.c
	$(CC) $(CFLAGS) $< -o $@

receiver: receiver.c
	$(CC) $(CFLAGS) $< -o $@

all: sender receiver

clean:
	$(RM) sender
	$(RM) receiver
