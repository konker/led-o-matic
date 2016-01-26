#ifndef __KONKER_BITFONT_BASIC__
#define __KONKER_BITFONT_BASIC__

char konker_bitfont_basic_metrics[128] =
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,5,5,4,4,1,2,2,3,3,2,4,1,4,5,3,3,3,4,3,4,3,4,4,2,4,3,3,3,4,4,4,4,4,4,3,3,4,4,3,4,4,3,5,5,5,4,5,4,4,3,4,5,5,5,5,4,2,4,2,3,4,2,4,3,3,3,4,2,3,3,1,2,3,2,5,4,4,3,3,3,3,4,4,3,5,3,3,3,3,3,3,4,4,};

char konker_bitfont_basic[128][6] = {
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x00
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x01
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x02
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x03
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x04
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x05
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x06
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x07
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x08
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x09
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x0A
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x0B
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x0C
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x0D
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x0E
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x0F
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x10
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x11
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x12
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x13
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x14
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x15
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x16
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x17
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x18
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x19
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x1A
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x1B
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x1C
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x1D
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x1E
{0x00,0x00,0x00,0x00,0x00,0x00}, //0x1F
{0x00,0x00,0x00,0x00,0x00,0x00}, //space
{0x00,0x80,0x80,0x80,0x00,0x80}, //!
{0xa0,0xa0,0x00,0x00,0x00,0x00}, //"
{0x00,0x50,0xf8,0x50,0xf8,0x50}, //#
{0x20,0x70,0xa0,0x70,0x28,0x70}, //$
{0x00,0x00,0x90,0x20,0x40,0x90}, //%
{0x00,0x40,0xa0,0x40,0xa0,0x50}, //&
{0x80,0x80,0x00,0x00,0x00,0x00}, //'
{0x40,0x80,0x80,0x80,0x80,0x40}, //(
{0x80,0x40,0x40,0x40,0x40,0x80}, //)
{0x00,0xa0,0x40,0xa0,0x00,0x00}, //*
{0x00,0x00,0x40,0xe0,0x40,0x00}, //+
{0x40,0x80,0x00,0x00,0x00,0x00}, //,
{0x00,0x00,0x00,0x70,0x00,0x00}, //-
{0x00,0x00,0x00,0x00,0x00,0x80}, //.
{0x10,0x20,0x20,0x40,0x40,0x80}, ///
{0x00,0x70,0x88,0xa8,0x88,0x70}, //0
{0x00,0x40,0xc0,0x40,0x40,0xe0}, //1
{0x00,0xc0,0x20,0x40,0x80,0xe0}, //2
{0x00,0xc0,0x20,0x40,0x20,0xc0}, //3
{0x00,0x20,0x60,0xa0,0xf0,0x20}, //4
{0x00,0xe0,0x80,0x60,0x20,0xc0}, //5
{0x00,0x60,0x80,0xe0,0x90,0x60}, //6
{0x00,0xe0,0x20,0x40,0x80,0x80}, //7
{0x00,0x60,0x90,0x60,0x90,0x60}, //8
{0x00,0x60,0x90,0x70,0x10,0x60}, //9
{0x00,0x00,0x40,0x00,0x40,0x00}, //:
{0x10,0x20,0x20,0x40,0x40,0x80}, ///
{0x00,0x20,0x40,0x80,0x40,0x20}, //<
{0x00,0x00,0xe0,0x00,0xe0,0x00}, //=
{0x00,0x80,0x40,0x20,0x40,0x80}, //>
{0x00,0x60,0x90,0x20,0x00,0x20}, //?
{0x00,0x00,0x60,0x90,0xb0,0xb0}, //@
{0x00,0x60,0x90,0x90,0xf0,0x90}, //A
{0x00,0xe0,0x90,0xe0,0x90,0xe0}, //B
{0x00,0x70,0x80,0x80,0x80,0x70}, //C
{0x00,0xe0,0x90,0x90,0x90,0xe0}, //D
{0x00,0xe0,0x80,0xc0,0x80,0xe0}, //E
{0x00,0xe0,0x80,0xc0,0x80,0x80}, //F
{0x00,0x70,0x80,0xb0,0x90,0x70}, //G
{0x00,0x90,0x90,0xf0,0x90,0x90}, //H
{0x00,0xe0,0x40,0x40,0x40,0xe0}, //I
{0x00,0x70,0x20,0x20,0xa0,0x40}, //J
{0x00,0x90,0xa0,0xc0,0xa0,0x90}, //K
{0x00,0x80,0x80,0x80,0x80,0xe0}, //L
{0x00,0x88,0xd8,0xa8,0x88,0x88}, //M
{0x00,0x88,0xc8,0xa8,0x98,0x88}, //N
{0x00,0x70,0x88,0x88,0x88,0x70}, //O
{0x00,0xe0,0x90,0xe0,0x80,0x80}, //P
{0x00,0x70,0x88,0xa8,0x90,0x68}, //Q
{0x00,0xe0,0x90,0xe0,0xa0,0x90}, //R
{0x00,0x70,0x80,0x60,0x10,0xe0}, //S
{0x00,0xe0,0x40,0x40,0x40,0x40}, //T
{0x00,0x90,0x90,0x90,0x90,0x60}, //U
{0x00,0x88,0x88,0x88,0x50,0x20}, //V
{0x00,0x88,0x88,0xa8,0xd8,0x50}, //W
{0x00,0x88,0x50,0x20,0x50,0x88}, //X
{0x00,0x88,0x50,0x20,0x20,0x20}, //Y
{0x00,0xf0,0x20,0x40,0x80,0xf0}, //Z
{0xc0,0x80,0x80,0x80,0x80,0xc0}, //[
{0x80,0x40,0x40,0x20,0x20,0x10}, //BS
{0xc0,0x40,0x40,0x40,0x40,0xc0}, //]
{0x40,0xa0,0x00,0x00,0x00,0x00}, //^
{0x00,0x00,0x00,0x00,0x00,0xf0}, //_
{0x80,0x40,0x00,0x00,0x00,0x00}, //`
{0x00,0x00,0x60,0xa0,0xa0,0x50}, //a
{0x00,0x80,0x80,0xc0,0xa0,0xc0}, //b
{0x00,0x00,0x60,0x80,0x80,0x60}, //c
{0x00,0x20,0x20,0x60,0xa0,0x60}, //d
{0x00,0x00,0x60,0xf0,0x80,0x60}, //e
{0x00,0x00,0x40,0x80,0xc0,0x80}, //f
{0x00,0x00,0x60,0xa0,0x60,0xc0}, //g
{0x00,0x00,0x80,0xc0,0xa0,0xa0}, //h
{0x00,0x80,0x00,0x80,0x80,0x80}, //i
{0x00,0x40,0x00,0x40,0x40,0x80}, //j
{0x00,0x00,0x80,0xa0,0xc0,0xa0}, //k
{0x00,0x80,0x80,0x80,0x80,0x40}, //l
{0x00,0x00,0x50,0xa8,0x88,0x88}, //m
{0x00,0x00,0x60,0x90,0x90,0x90}, //n
{0x00,0x00,0x60,0x90,0x90,0x60}, //o
{0x00,0x00,0xc0,0xa0,0xc0,0x80}, //p
{0x00,0x00,0x40,0xa0,0x60,0x20}, //q
{0x00,0x00,0x40,0xa0,0x80,0x80}, //r
{0x00,0x00,0x60,0xc0,0x20,0xc0}, //s
{0x00,0x40,0xf0,0x40,0x40,0x30}, //t
{0x00,0x00,0x90,0x90,0x90,0x60}, //u
{0x00,0x00,0xa0,0xa0,0xa0,0x40}, //v
{0x00,0x00,0x88,0x88,0xa8,0x50}, //w
{0x00,0x00,0x00,0xa0,0x40,0xa0}, //x
{0x00,0x00,0xa0,0xe0,0x20,0xc0}, //y
{0x00,0x00,0xe0,0x20,0x40,0xe0}, //z
{0x60,0x40,0xc0,0x40,0x40,0x60}, //{
{0x20,0x20,0x20,0x20,0x20,0x20}, //|
{0xc0,0x40,0x60,0x40,0x40,0xc0}, //}
{0x00,0x00,0x50,0xa0,0x00,0x00}, //~
{0x84,0x00,0x30,0x30,0x00,0x84} //7F

};

#endif