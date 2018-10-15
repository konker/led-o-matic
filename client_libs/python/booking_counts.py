# -*- coding: utf-8 -*-
import time
import requests

from gtts import gTTS
from pygame import mixer

from led_o_matic import LedOMatic

DATA_DELAY_SECS = 10
ANIM_DELAY_SECS = 0.5
STAR = '.'
TARGET = '9000000'

booking_milestones = {
    999500: "Onwards.",
    999600: "Looking great!",
    999700: "My word, this is cool!",
    999800: "I think I shall have a cup of tea",
    999900: "By george, we are almost there!",
    999980: "Gather round, grab the bubbly.",
}


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
            bookings = eval(response.text)['count']

            if bookings != None and bookings != last_bookings:
                # Get bookings count
                bookings_disp = "{:,}".format(bookings)
                #print(bookings_disp)

                # Print bookings count to sign
                animate_number_change(last_bookings_disp, bookings_disp)
                lomd.center(0)

                """
                milestones = booking_milestones.keys()

                for milestone in sorted(milestones, reverse=True):
                    if int(bookings) >= milestone and int(last_bookings) < milestone:
                        difference = int(TARGET) - milestone

                        # Generate text to speech phrase
                        text = booking_milestones[milestone] + "..." + "Less than " + str(difference) + " bookings to go!"
                        tts = gTTS(text=text, lang="en")
                        tts.save("audio.mp3")

                        # Playback TTS audio
                        mixer.init()
                        mixer.music.load("audio.mp3")
                        mixer.music.play()
                        
                        break

                if (int(bookings) >= int(TARGET) and int(last_bookings) < int(TARGET)):
                    animate_flashing(24)
                    mixer.init()
                    mixer.music.load("HeavenlyMotion.mp3")
                    mixer.music.play()

                """
            last_bookings = bookings
            last_bookings_disp = bookings_disp
            time.sleep(DATA_DELAY_SECS)
        except Exception as ex:
            print("Warning: caught error", ex)


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
        #print(s)
        lomd.text(0, s)
        lomd.center(0)

        idx = to_c_anim.index(STAR)
        to_c_anim[idx] = to_c[idx]
        time.sleep(ANIM_DELAY_SECS)

    s = "".join(to_c_anim)
    #print(s)
    #print('------')
    lomd.text(0, s)
    lomd.center(0)

if __name__ == '__main__':
    main()


