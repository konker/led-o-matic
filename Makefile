CC = gcc
CFLAGS = ${ENV_CFLAGS} -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -g -fno-builtin \
		 -I../konker_ultrathin_led_matrix -L../konker_ultrathin_led_matrix -lkulm \
		 -I../hexfont -L../hexfont -lhexfont \
		 -I../tinyutf8 -L../tinyutf8 -ltinyutf8 \
		 -I../inih -L../inih -linih \
		 -lpthread -lm -lwiringPi

led-o-maticd: led-o-maticd.c
	$(CC) -o led-o-maticd $< $(CFLAGS)

clean:
	rm -rf *.o led-o-maticd

all: led-o-maticd
.PHONY: all clean
