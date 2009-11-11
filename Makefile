test: test.cpp test2.cpp mingl.h *.h testtex.cpp
	g++ -msse -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Werror -g test.cpp test2.cpp testtex.cpp -lX11 -o test

testtex.cpp: testtex.png
	convert testtex.png testtex.rgba
	xxd -i testtex.rgba testtex.cpp
	rm testtex.rgba
