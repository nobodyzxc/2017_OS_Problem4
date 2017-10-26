all:
	g++ -std=c++11 -c request.cc ui.cc bank.cc
	g++ -std=c++11 -o a.exe main.cc request.o bank.o ui.o -pthread
	#g++ -c request.cc ui.cc bank.cc
	#g++ -o a.exe main.cc request.o bank.o ui.o -pthread
clean:
	rm a.exe *.o
