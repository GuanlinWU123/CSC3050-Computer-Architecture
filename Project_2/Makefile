OBJ = main.o phase1.o memory.o phase2.o

Ass2:$(OBJ)
	g++ main.o phase1.o memory.o phase2.o -o simulator
main.o:main.cpp
	g++ -c main.cpp -o main.o
phase1.o:phase1.cpp phase1.h
	g++ -c phase1.cpp -o phase1.o
memory.o:memory.cpp memory.h
	g++ -c memory.cpp -o memory.o
phase2.o:phase2.cpp phase2.h
	g++ -c phase2.cpp -o phase2.o

.PHONY:clean

clean:
	rm *.o Ass2