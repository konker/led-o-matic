CC = gcc
CFLAGS = ${ENV_CFLAGS} -std=c99 -D_POSIX_C_SOURCE=200112L -Wall -g \
		 -I../konker_ultrathin_led_matrix -L../konker_ultrathin_led_matrix -lkulm \
		 -I../hexfont -L../hexfont -lhexfont \
		 -I../tinyutf8 -L../tinyutf8 -ltinyutf8 \
		 -lpthread -fno-builtin -lm -lwiringPi

led-o-maticd: led-o-maticd.c
	$(CC) -o led-o-maticd $< $(CFLAGS)

clean:
	rm -rf *.o led-o-maticd

all: led-o-maticd
.PHONY: all clean
