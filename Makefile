CC = gcc
CFLAGS = -Wall -g -I../ultrathin_led_matrix/include -L../ultrathin_led_matrix -lulm -lpthread -fno-builtin -lm -lwiringPi

led-o-maticd: led-o-maticd.c
	$(CC) -o led-o-maticd $< $(CFLAGS)

clean:
	rm -rf *.o led-o-maticd

all: led-o-maticd
.PHONY: all clean
