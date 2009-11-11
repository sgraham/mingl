#!/usr/bin/env python

import sys
import os
import re

def cmd(cmd):
    print ". running: '%s'" % cmd
    ret = os.system(cmd)
    if ret != 0:
        print "FAILED"
        raise SystemExit()

def test(wantDebug=True):
    """build simple test exe and run it"""
    cmd("g++ -msse -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Werror -g %s test.cpp test2.cpp testtex.cpp -lX11 -o test"
            % ("-DWANT_DEBUG_TEST" if wantDebug else ""))
    cmd("./test")

def tex():
    """convert our silly test png into a cpp file for ease of referencing"""
    cmd("convert testtex.png testtex.rgba")
    cmd("xxd -i testtex.rgba testtex.cpp")
    cmd("rm testtex.rgba")

def dist():
    """build separate source files into one combined header for easy distribution"""
    print ". building combined mingl.h"
    out = open("mingl.h", "w")
    def process(fn):
        for line in open(fn).readlines():
            if line.startswith("//STRIP"): continue
            mo = re.search('#include "(.*)"', line)
            if mo and mo.group(1) != "mingl.h": # avoid example code include
                process(mo.group(1))
            else:
                print >>out, line,
                #print >>out, "/* mingl.h, edit original instead */", line,
    process("mingl_debug.h")
    out.close()
    test(wantDebug=False)

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
