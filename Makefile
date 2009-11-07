test: test.cpp mingl.h mingl.cpp 
	g++ -Wall -Wextra -Werror -g test.cpp mingl.cpp -lX11 -o test

testtex.cpp: testtex.png
	convert testtex.png testtex.rgba
	xxd -i testtex.rgba testtex.cpp
	rm testtex.rgba
