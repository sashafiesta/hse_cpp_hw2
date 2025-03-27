all:
	g++ -O3 -I. -o tests.o symbolic.hpp tests.cpp
	g++ -O3 -I. -o differentiator.o symbolic.hpp differentiator.cpp

test:
	g++ -O3 -I. -o tests.o symbolic.hpp tests.cpp
	./tests.o
diff:
	g++ -O3 -I. -o differentiator.o symbolic.hpp differentiator.cpp
	./differentiator.o

clean:
	rm -f ./*.o