CC = gcc
CFLAGS = -Wall -g -I../konker_ultrathin_led_matrix -L../konker_ultrathin_led_matrix -lkulm -lpthread -fno-builtin -lm -lwiringPi

led-o-maticd: led-o-maticd.c
	$(CC) -o led-o-maticd $< $(CFLAGS)

clean:
	rm -rf *.o led-o-maticd

all: led-o-maticd
.PHONY: all clean
