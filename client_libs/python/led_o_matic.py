#!/usr/bin/env python3

"""
Python library for interacting with the led-o-maticd daemon
Author: Konrad Markus <konker@luxvelocitas.com>
"""
import socket

BUFSIZE = 1024


class LedOMatic(object):

    def __init__(self, port, ip='127.0.0.1'):
        self.port = port
        self.ip = ip

        self.sock = socket.socket(socket.AF_INET, # Internet
                                  socket.SOCK_DGRAM) # UDP


    def _write_cmd(self, cmd):
        self.sock.sendto(cmd.encode(), (self.ip, self.port))


    def _write_read_cmd(self, cmd):
        self.sock.sendto(cmd.encode(), (self.ip, self.port))
        data, fromaddr = self.sock.recvfrom(BUFSIZE)
        return data


    def on(self):
        self._write_cmd("on\n")


    def off(self):
        self._write_cmd("off\n")


    def modulate(self, *args):
        if (len(args) > 1):
            self._write_cmd("modulate:%s\n" % (args[0]))
        else:
            return self._write_read_cmd("modulate\n")
        self._write_cmd("modulate:%s\n" % n)


    def start(self, segment):
        self._write_cmd("start:%s\n" % segment)


    def stop(self, segment):
        self._write_cmd("stop:%s\n" % segment)


    def reverse(self, segment):
        self._write_cmd("reverse:%s\n" % segment)


    def clear(self, segment=-1):
        if (segment != -1):
            self._write_cmd("clear:%s\n" % segment)
        else:
            self._write_cmd("clear\n")


    def text(self, *args):
        if (len(args) > 1):
            self._write_cmd("text:%s:%s\n" % (args[0], args[1]))
        else:
            return str(self._write_read_cmd("text:%s\n" % (args[0])))


    def position(self, *args):
        if (len(args) > 2):
            self._write_cmd("position:%s:%s:%s\n" % (args[0], args[1], args[2]))
        else:
            csv = self._write_read_cmd("position:%s\n" % (args[0])).decode("utf-8")
            (x, y) = csv.split(',')
            return (int(float(x)), int(float(y)))


    def center(self, segment):
        csv = self._write_read_cmd("center:%s\n" % segment).decode("utf-8")
        (h, v) = csv.split(',')
        return (int(float(h)), int(float(v)))


    def speed(self, *args):
        if (len(args) > 2):
            self._write_cmd("speed:%s:%s:%s\n" % (args[0], args[1], args[2]))
        else:
            csv = self._write_read_cmd("speed:%s\n" % (args[0])).decode("utf-8")
            (h, v) = csv.split(',')
            return (int(float(h)), int(float(v)))


    def exit(self):
        self._write_cmd("exit\n")



if __name__ == '__main__':
    server = LedOMatic(7890)
    server.on()
    server.start(0)
    server.text(0, "KONKER IS KING!!")

