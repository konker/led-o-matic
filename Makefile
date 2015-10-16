CC = g++
CC3 = gcc
CFLAGS = -Wall -g -std=c++11 -lwiringPi -IUltrathinLEDMatrix -LUltrathinLEDMatrix -lUltrathinLEDMatrix
CFLAGS2 = -Wall -g -std=c++11
CFLAGS3 = -Wall -g

led-o-matic: led-o-matic.cpp
	$(CC) -o led-o-matic $< $(CFLAGS)

test-daemon: test-daemon.c
	$(CC3) -o test-daemon $< $(CFLAGS3)

test-udp-daemon: test-udp-daemon.c
	$(CC3) -o test-udp-daemon $< $(CFLAGS3)

clean:
	rm -rf *.o led-o-matic test-daemon test-udp-daemon

all: led-o-matic test-daemon test-udp-daemon
.PHONY: all clean
