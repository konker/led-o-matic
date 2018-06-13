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

        self.sock = socket.socket(socket.AF_INET, # Internet
                                  socket.SOCK_DGRAM) # UDP


    def _write_cmd(self, cmd):
        self.sock.sendto(cmd.encode(), (self.ip, self.port))


    def on(self):
        self._write_cmd("on\n")


    def off(self):
        self._write_cmd("off\n")

    def modulate(self, n):
        self._write_cmd("modulate:%s\n" % n)


    def start(self, segment):
        self._write_cmd("start:%s\n" % segment)


    def stop(self, segment):
        self._write_cmd("stop:%s\n" % segment)


    def reverse(self, segment):
        self._write_cmd("reverse:%s\n" % segment)


    def clear(self, segment):
        self._write_cmd("clear:%s\n" % segment)


    def text(self, segment, text):
        self._write_cmd("text:%s:%s\n" % (segment, text))


    def position(self, segment, x):
        self._write_cmd("position:%s:%s\n" % (segment, x))

    def speed(self, segment, speed):
        self._write_cmd("speed:%s:%s\n" % (segment, speed))


    def exit(self):
        self._write_cmd("exit\n")



if __name__ == '__main__':
    server = LedOMatic(7890)
    server.on()
    server.start(0)
    server.text(0, "KONKER IS KING!!")

