proxy-dll-helper.exe : main.o
	g++ main.o -oproxy-dll-helper.exe

main.o : main.cpp
	g++ -c main.cpp