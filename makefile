#
#
#
#
#

kegger: kegger.o
	g++ kegger.o -o kegger 

kegger.o: kegger.cpp
	g++ -c kegger.cpp -lphtread