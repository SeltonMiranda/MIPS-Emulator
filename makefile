CPPFLAGS = -std=c++23 -Wall -pedantic -g -I/usr/include/boost
OBJECTS = build/main.o build/Tokenizer.o build/Engine.o build/CPU.o

EXECUTABLE = emulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo building executable...
	g++ $(CPPFLAGS) -o $@ $^

	
build/%.o: %.cpp | build
	@echo building objects...
	@mkdir -p build
	g++ $(CPPFLAGS) -c $< -o $@

run:
	./$(EXECUTABLE)

clean:
	@echo cleaning...
	rm -rf $(EXECUTABLE) *.gch build/*