# -*- coding: utf-8 -*-
import time
import requests

from led_o_matic import LedOMatic

# Set up URL and headers
url = 'https://counter.analytics.maas.global/current'
headers = {'x-api-key': 'fWFo8rlFYm849XgetJMMp1q1sgcA10TU7UerCqlN'}

# Connect to sign
led_o_matic = LedOMatic(ip='10.0.1.117', port=7890)

# TODO: signal handler and guard variable
while(True):
    # Switch the sign off
    #led_o_matic.off()
    #led_o_matic.modulate(10000)

    # Get the bookings response
    response = requests.get(url, headers=headers)
    bookings = eval(response.text)

    # Get bookings count
    bookings_count = "Trips: " + "{:,}".format(bookings['count'])
    print(bookings_count)

    # Print bookings count to sign
    led_o_matic.position(0, 6)
    led_o_matic.text(0, bookings_count)

    # Switch the sign on
    #led_o_matic.on()
    led_o_matic.modulate(0)

    time.sleep(10)


