# Makefile for the smash program
CXX = g++
CXXFLAGS = --std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CXX)
OBJS = ttftps.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o ttftps $(OBJS)
# Creating the object files
ttftps.o: ttftps.cpp Headers.hpp Server.hpp
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* Server
