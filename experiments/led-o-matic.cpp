/**
 * led-o-matic
 * The multi-function scrolling LED display
 * (Although greed is not good actually)
 *
 * Author: Konrad Markus <konker@luxvelocitas.com>
 *
*/

/*[TODO]
    - use proper pthreads?
    - starting and clean up of threads
    - animate method of UltrathinLEDMatrix class
        - one for text1, one for text2?
        - what about 1 line of big text?

    - proper checking for arduino, etc
        - should compile on arduino and rpi/g++

    - should we be writing this in something like python?
        - nice to keep arduino compat (i.e. C/C++)
        - is wrapping C++ for python too much of a hassle?

    - Keep the low(er)-level hardware code in C++
        - internet fetching, etc in python/node?
        - C++ code can act as a "service" to accept text and display it
            - how would this work?
                - ???
*/

#include <iostream>
#include <wiringPi.h>
#include <UltrathinLEDMatrix.h>

#define WIDTH 64
#define HEIGHT 16
#define ROW_WIDTH (WIDTH / 8)

#define row_offset(y) (ROW_WIDTH*y)
#define buf_offset(x, y) (row_offset(y)+x/8)

uint8_t display_buffer[HEIGHT * ROW_WIDTH];
UltrathinLEDMatrix matrix(8, 9, 7, 0, 2, 3, 13, 14);

PI_THREAD(scanner)
{
    while (1) {
        matrix.scan();
    }
    return NULL;
}


int main(int argc, char **argv) {
    wiringPiSetup();

    matrix.begin(display_buffer, WIDTH, HEIGHT);

    matrix.set_text1("I AM KONKER!");
    matrix.set_text1_speed(1.0f);

    matrix.set_text2("tHE qUICK bROWN fOX jUMPED oVER tHE lAZY dOGS?");
    matrix.set_text2_speed(1.5f);

    int scan_thread = piThreadCreate(scanner);
    if (scan_thread != 0)
        std::cout << "it didn't start" << std::endl;

    while (1) {
        delay(100);
        matrix.tick();
    }
}
