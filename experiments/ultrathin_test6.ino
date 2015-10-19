 /**
 * Author: Konrad Markus <konker@luxvelocitas.com>
 */


#include <TimerOne.h>
#include <UltrathinLEDMatrix.h>
//#include <konker_bitfont_basic.h>

#define WIDTH 64
#define HEIGHT 16
#define ROW_WIDTH (WIDTH / 8)

#define row_offset(y) (ROW_WIDTH*y)
#define buf_offset(x, y) (row_offset(y)+x/8)

#define FONT konker_bitfont_basic

uint8_t display_buffer[HEIGHT * ROW_WIDTH];

UltrathinLEDMatrix matrix(4, 5, 6, 7, 8, 9, 10, 11);

char smile[8] = { 0x00, 0x66, 0x66, 0x00, 0x00, 0x81, 0x42, 0x3c };
char angryface[8] = { 0x00, 0x66, 0x66, 0x00, 0x18, 0x24, 0x42, 0x81 };
char laughing[8] = { 0x00, 0x66, 0x66, 0x00, 0xff, 0x81, 0x42, 0x3c };
char frown[8] = { 0x00, 0x66, 0x66, 0x00, 0x00, 0x3c, 0x42, 0x81 };
char winkleft[8] = { 0x00, 0x06, 0x66, 0x00, 0x00, 0x81, 0x42, 0x3c };
char winkright[8] = { 0x00, 0x60, 0x66, 0x00, 0x00, 0x81, 0x42, 0x3c };
char tongueout[8] = { 0x00, 0x66, 0x66, 0x00, 0x00, 0x7e, 0x0a, 0x04 };
char expressionless[8] = { 0x00, 0x66, 0x66, 0x00, 0x00, 0xff, 0x00, 0x00 };
char blink[8] = { 0x00, 0x00, 0x66, 0x00, 0x00, 0x81, 0x42, 0x3c };

char heart[8] = { 0x66, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00 };
char heartoutline[8] = { 0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18, 0x00 };
char arrowright[8] = { 0x00, 0x20, 0x60, 0xff, 0xff, 0x60, 0x20, 0x00 };
char arrowleft[8] = { 0x00, 0x04, 0x06, 0xff, 0xff, 0x06, 0x04, 0x00 };
char target[8] = { 0x3c, 0x7e, 0xc3, 0xdb, 0xdb, 0xc3, 0x7e, 0x3c };
char ball[8] = { 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c };
char clock[8] = { 0x3c, 0x52, 0x91, 0x91, 0x8f, 0x81, 0x42, 0x3c };
char bell[8] = { 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x7e, 0x00, 0x18 };
char speaker[8] = { 0x03, 0x07, 0x3f, 0x3f, 0x3f, 0x3f, 0x07, 0x03 };
char sound[8] = { 0x00, 0x40, 0x80, 0x00, 0xc0, 0x00, 0x80, 0x40 };

volatile char c = '0';
volatile int16_t p1 = 0;
volatile int16_t p2 = 0;
volatile int16_t l1 = 0;
volatile int16_t l2 = 0;

void setup() {
    matrix.begin(display_buffer, WIDTH, HEIGHT);
    
    matrix.clear();
    matrix.set_text1("konker@gmail.com Hi Mr Brown. (Teenage kicks?) The Quick Brown Fox Jumped Over The Lazy Dogs!");
    l1 = matrix.get_text1_pixel_len();
    matrix.set_text2("tHE qUICK bROWN fOX jUMPED oVER tHE lAZY dOGS?");
    l2 = matrix.get_text2_pixel_len();

    /*
    matrix.render_text1(0);
    matrix.render_text2(0);
    */
    
    /*
    matrix.write_char(0,  0, 'A');
    matrix.write_char(6,  0, 'B');
    matrix.write_char(12, 0, 'C');
    matrix.write_char(18, 0, 'D');
    */
    //matrix.write_char(24, 0, 'E');
    //matrix.set_region(konker_bitfont_basic[1], 0,0, 8,8);
/*
    matrix.set_region(font8x8_basic[75], 0,4, 8,8);
    matrix.set_region_hflip(blink, 9,4, 8,8);
    matrix.set_region(font8x8_basic[72], 20,4, 8,8);
    matrix.set_region(font8x8_basic[72], 24,4, 8,8);
*/
    // animate to run every 0.15 seconds
    Timer1.initialize(100000);
    Timer1.attachInterrupt(animate);
    //matrix.render_text1(-1, 0);
    //matrix.render_text2(0, 8);

    //Serial.begin(9600);
}

void loop() {
    matrix.scan();
}

void animate() {
    /*
    Serial.print(matrix.get_text1_len());
    Serial.print(",");
    Serial.print(p1);
    Serial.print(",");
    Serial.print(-1*matrix.get_text1_len());
    Serial.print("|");
    */
    
    /*
    Serial.print(l2);
    Serial.print(",");
    Serial.print(p2);
    */
    //Serial.print("\n");
    
    p1 = (p1 - 1);
    if (p1 < -1*l1) {
        p1 = WIDTH;
        matrix.clear_region(0,0, WIDTH,HEIGHT);
    }
    matrix.render_text1(p1, 0);

    p2 = (p2 - 2);
    if (p2 < -l2) {
        p2 = WIDTH;
    }
    matrix.render_text2(p2, 8);

    //matrix.reverse();
    /*
    c = (c + 1) % 127;
    matrix.write_char(0,  0, c);
    matrix.write_char(6,  0, c);
    matrix.write_char(12, 0, c);
    matrix.write_char(18, 0, c);
    */
}

