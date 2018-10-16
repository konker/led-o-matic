# -*- coding: utf-8 -*-
import time
import requests

from led_o_matic import LedOMatic

DATA_DELAY_SECS = 10
ANIM_DELAY_SECS = 0.5
STAR = '.'


# Set up URL and headers
url = 'https://counter.analytics.maas.global/current'
headers = {'x-api-key': 'fWFo8rlFYm849XgetJMMp1q1sgcA10TU7UerCqlN'}

# Connect to sign
lomd = LedOMatic(ip='10.0.1.52', port=7890)


def main():
    last_bookings = 0
    last_bookings_disp = "0"

    # TODO: signal handler and guard variable
    while(True):
        bookings = None
        # Get the bookings response
        try:
            response = requests.get(url, headers=headers)
        except Exception as ex:
            print("Warning: caught error", ex)
            time.sleep(DATA_DELAY_SECS)
            next

        bookings = eval(response.text)['count']

        if bookings != None and bookings != last_bookings:
            bookings_disp = "{:,}".format(bookings)
            #print(bookings_disp)

            # Print bookings count to sign
            animate_number_change(last_bookings_disp, bookings_disp)

        last_bookings = bookings
        last_bookings_disp = bookings_disp
        time.sleep(DATA_DELAY_SECS)


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

    to_c_anim = to_c.copy()

    # Change all different digits to a * character
    for idx, val in enumerate(to_c):
        if (len(from_c) > idx and from_c[idx] == to_c[idx]):
            to_c_anim[idx] = to_c[idx]
        else:
            to_c_anim[idx] = STAR

    while STAR in to_c_anim:
        s = "".join(to_c_anim)
        lomd.text(0, s)
        #lomd.center(0)

        idx = to_c_anim.index(STAR)
        to_c_anim[idx] = to_c[idx]
        time.sleep(ANIM_DELAY_SECS)

    s = "".join(to_c_anim)
    lomd.text(0, s)
    lomd.center(0, 1, 1)

if __name__ == '__main__':
    main()


