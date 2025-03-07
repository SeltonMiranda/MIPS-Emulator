CPPFLAGS =-std=c++23 -Wall -pedantic -g0 -ffunction-sections -fdata-sections -Wl,--gc-sections -I/usr/include/boost

OBJECTS = build/main.o build/Tokenizer.o build/Engine.o build/CPU.o

EXECUTABLE = emulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo building executable...
	g++ $(CPPFLAGS) -o $@ $^

	
build/%.o: %.cpp
	@echo building objects...
	@mkdir -p build
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	@echo cleaning up...
	rm -rf $(EXECUTABLE) *.gch build/