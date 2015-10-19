#!/usr/bin/env python3

"""
Python library for interacting with the led-o-maticd daemon
Author: Konrad Markus <konker@luxvelocitas.com>
"""
import socket

class LedOMatic(object):
    def __init__(self, port, ip='127.0.0.1'):
        self.port = port
        self.ip = ip
        self._text1 = ""
        self._speed1 = 1.0
        self._text2 = ""
        self._speed2 = 1.0

        self.sock = socket.socket(socket.AF_INET, # Internet
                                  socket.SOCK_DGRAM) # UDP


    def _write_cmd(self, cmd):
        self.sock.sendto(cmd.encode(), (self.ip, self.port))


    def on(self):
        self._write_cmd("on\n")


    def off(self):
        self._write_cmd("off\n")


    def start(self):
        self._write_cmd("start\n")


    def stop(self):
        self._write_cmd("stop\n")


    def reverse(self):
        self._write_cmd("reverse\n")


    def clear(self):
        self._write_cmd("clear\n")


    def text1(self, text):
        self._text1 = text
        self._write_cmd("text1:%s\n" % self._text1)


    def speed1(self, speed):
        self._speed1 = speed
        self._write_cmd("speed1:%f\n" % self._speed1)


    def text2(self, text):
        self._text2 = text
        self._write_cmd("text2:%s\n" % self._text2)


    def speed2(self, speed):
        self._speed2 = speed
        self._write_cmd("speed2:%f\n" % self._speed2)

    def __repr__(self):
        return ("(%f)[%s], (%f)[%s]" % (self._speed1, self._text1, self._speed2, self._text2))


if __name__ == '__main__':
    server = LedOMatic(7890)
    server.on()
    server.start()
    server.text1("KONKER IS KING!!")
    server.speed1(0.5)

