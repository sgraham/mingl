test: test.cpp mingl.h mingl.cpp 
	g++ -msse -Wall -fstrict-aliasing -Wextra -Wno-unused-parameter -Werror -g test.cpp testtex.cpp mingl.cpp -lX11 -o test

testtex.cpp: testtex.png
	convert testtex.png testtex.rgba
	xxd -i testtex.rgba testtex.cpp
	rm testtex.rgba
