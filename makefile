CPPFLAGS = -std=c++23 -Wall -pedantic -g -I/usr/include/boost
OBJECTS = main.o Tokenizer.o Emulator.o CPU.o

EXECUTABLE = emulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	g++ $(CPPFLAGS) -o $@ $^

%.o: %.cpp 
	g++ $(CPPFLAGS) -c $< -o $@

run:
	./$(EXECUTABLE)

clean:
	rm -f *.o $(EXECUTABLE) *.gch