# -*- coding: utf-8 -*-
import time
import requests

import time
from led_o_matic import LedOMatic

DELAY_SECS = 2
BASE_N = 99997
ANIM_DELAY_SECS = 0.5
STAR = '.'
TARGET = '100000'

# Set up URL and headers
url = 'https://counter.analytics.maas.global/current'
headers = {'x-api-key': 'fWFo8rlFYm849XgetJMMp1q1sgcA10TU7UerCqlN'}

# Connect to sign
lomd = LedOMatic(ip='10.0.1.117', port=7890)


def main():
    last_bookings = 0

    # TODO: signal handler and guard variable
    while(True):
        # Switch the sign off
        #lomd.off()
        lomd.modulate(10000)

        # Get the bookings response
        response = requests.get(url, headers=headers)
        bookings = eval(response.text)['count']

        # Switch the sign on
        #lomd.on()
        lomd.modulate(0)

        # Get bookings count
        #bookings_count = "Trips: " + "{:,}".format(bookings['count'])
        #print(bookings_count)
        print(bookings)

        # Print bookings count to sign
        #lomd.text(0, bookings_count)
        animate_number_change(last_bookings, bookings)
        lomd.center(0)

        last_bookings = bookings
        time.sleep(10)


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
    from_c = list(str(from_n))
    to_c = list(str(to_n))

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

