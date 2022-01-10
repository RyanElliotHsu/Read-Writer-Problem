output: main.cpp 
	g++ -o reader reader.cpp
	g++ -o writer writer.cpp
	g++ -o header header.hpp
	g++ -o output main.cpp

main.o: main.cpp header.hpp
	g++ -c main.cpp

run:
	./output $(tp) $(rt) $(wt)

reader.o: reader.cpp header.hpp
	g++ -c reader.cpp

writer.o: writer.cpp header.hpp
	g++ -c writer.cpp

header.o: header.hpp
	g++ -c header.hpp
	
clean:
	rm *.o output