result: main.o ff_lib.a
	g++ main.o -o out -L. ff_lib.a -std=c++17 -pthread
	
main.o: main.cpp
	g++ -c main.cpp -O2
	
ff_lib.a: FileFinder.o
	ar rcs ff_lib.a FileFinder.o
	
FileFinder.o: src/FileFinder.cpp
	g++ -c src/FileFinder.cpp -o FileFinder.o -std=c++17 -O2
	
clean:
	rm *.o *.a out