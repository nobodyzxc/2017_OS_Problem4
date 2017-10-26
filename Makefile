all:
	g++ -c request.cc ui.cc bank.cc
	g++ -o a.exe main.cc request.o bank.o ui.o -pthread
clean:
	rm a.exe *.o
