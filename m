#!/usr/bin/env python

import sys
import os
import re
import platform

def cmd(cmd):
    print ". running: '%s'" % cmd
    ret = os.system(cmd)
    if ret != 0:
        print "FAILED"
        raise SystemExit()

def buildAndRun(srcs, wantDebug=True):
    """build and run simple tests. srcs shouldn't have extension. -o is to first srcs"""
    plat = platform.uname()[0]
    if plat == "Linux":
        cmd("g++ -msse -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Werror -g %s %s -lX11 -o %s"
                % ("" if wantDebug else "-O3 -DWANT_DIST_TEST",
                   ' '.join([x+".cpp" for x in srcs]),
                   srcs[0]))
        cmd("./" + srcs[0])
    else:
        print "don't know how to build and run"

def test(wantDebug=True):
    """build simple test exe and run it"""
    buildAndRun(["test", "test2", "testtex"])

def tex():
    """convert our silly test png into a cpp file for ease of referencing"""
    cmd("convert testtex.png testtex.rgba")
    cmd("xxd -i testtex.rgba testtex.cpp")
    os.unlink("testtex.rgba")

def dist(withLines=False):
    """build separate source files into one combined header for easy distribution"""
    if not os.path.exists("dist"): os.mkdir("dist")
    print ". building combined mingl.h"
    out = open("dist/mingl.h", "w")
    def process(fn):
        if withLines:
            print >>out, '#line 1 "%s"' % fn
        curline = 1
        for line in open(fn).readlines():
            if line.startswith("//STRIP"): continue
            mo = re.search('#include "(.*)"', line)
            if mo and mo.group(1) != "mingl.h": # avoid example code include
                process(mo.group(1))
                if withLines:
                    print >>out, '#line %d "%s"' % (curline, fn)
            else:
                print >>out, line,
                #print >>out, "/* mingl.h, edit original instead */", line,
            curline += 1
    process("mingl_debug.h")
    out.close()

def ex(num):
    dist(withLines=True)
    buildAndRun(["examples/ex%02d" % int(num), "testtex"])

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
    elif arg == "ex":
        ex(sys.argv[2])
    else:
        print "usage: m {test|tex|dist}"

if __name__ == "__main__":
    main()
