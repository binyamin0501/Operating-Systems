# Makefile for the ttftps program
CC = g++
CFLAGS = -g -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC)
OBJS =  ttftps.o
RM = rm -f
TARGET = ttftps

# Creating the  executable
$(TARGET): $(OBJS)
	$(CCLINK) $(CFLAGS) -o ttftps $(OBJS)

# Creating the object files
ttftps.o: ttftps.cpp ttftps.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*