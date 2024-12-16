CPPFLAGS = -std=c++23 -Wall -pedantic -g
EXECUTABLE = emulator

.phony: all clean run

all: $(EXECUTABLE)

$(EXECUTABLE): main.cpp
	g++ $< $(CPPFLAGS) -o $@

run:
	./$(EXECUTABLE)

clean:
	rm -f *.o $(EXECUTABLE) *.gch