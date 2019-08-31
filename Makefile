main : TSP.o main.o
	gcc --std=c99 -Wall main.o TSP.o -o main

main.o : main.c TSP.c TSP.h
	gcc -c --std=c99 -Wall main.c

TSP.o : TSP.h TSP.c
	gcc -c --std=c99 -Wall TSP.c

clean :
	rm main main.o TSP.o