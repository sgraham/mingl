#!/usr/bin/env python
#
# See mingl.h for copyright and documentation.

import sys
import os

def cmd(cmd):
    print cmd
    ret = os.system(cmd)
    if ret != 0:
        print "FAILED"
        raise SystemExit()

def test():
    """build simple test exe and run it"""
    cmd("g++ -msse -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Werror -g test.cpp test2.cpp testtex.cpp -lX11 -o test")
    cmd("./test")

def tex():
    """convert our silly test png into a cpp file for ease of referencing"""
    cmd("convert testtex.png testtex.rgba")
    cmd("xxd -i testtex.rgba testtex.cpp")
    cmd("rm testtex.rgba")

def dist():
    """build separate source files into one combined header for easy distribution"""

def main():
    if len(sys.argv) < 2:
        arg = "test"
    else:
        arg = sys.argv[1]
    if arg == "test":
        test()
    elif arg == "tex":
        tex()
    elif arg == "dist":
        dist()
    else:
        print "usage: m {test|tex|dist}"

if __name__ == "__main__":
    main()
