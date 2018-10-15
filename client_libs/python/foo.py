#!/usr/bin/env python3


import time
from led_o_matic import LedOMatic

DELAY_SECS = 2
BASE_N = 99997
ANIM_DELAY_SECS = 0.5
STAR = '.'
TARGET = '100000'

lomd = LedOMatic(7890, '10.0.1.117')


def main():
    last_n = str(BASE_N)

    for n in range(100):
        to_n = str(BASE_N + n)
        animate_number_change(last_n, to_n)
        if (to_n == TARGET):
            animate_flashing(24)

        print(to_n)
        print(TARGET)
        print(to_n == TARGET)
        last_n = to_n
        time.sleep(DELAY_SECS)


# Repeatedly flash the display
def animate_flashing(n):
    for i in range(n):
        lomd.reverse(0)
        time.sleep(ANIM_DELAY_SECS)


# Scroll the text until it reaches the given x coordinate
def animate_scroll_to(s, x):
    pass


# Animate a transition from one number to another
def animate_number_change(from_n, to_n):
    from_c = list(from_n)
    to_c = list(to_n)

    #print(from_c)
    #print(to_c)

    to_c_anim = to_c.copy()

    # Change all different digits to a * character
    for idx, val in enumerate(to_c):
        if (len(from_c) > idx and from_c[idx] == to_c[idx]):
            to_c_anim[idx] = to_c[idx]
        else:
            to_c_anim[idx] = STAR

    while STAR in to_c_anim:
        s = "".join(to_c_anim)
        print(s)
        lomd.text(0, s)
        lomd.center(0)

        idx = to_c_anim.index(STAR)
        to_c_anim[idx] = to_c[idx]
        time.sleep(ANIM_DELAY_SECS)

    s = "".join(to_c_anim)
    print(s)
    print('------')
    lomd.text(0, s)
    lomd.center(0)



if __name__ == '__main__':
    main()
