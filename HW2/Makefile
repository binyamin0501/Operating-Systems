CC=g++
CFLAGS=-std=c++11 -g -Wall -Werror -pedantic-errors -DNEBUG -pthread
CCLINK=$(CC)
OBJS=main.o Bank-Account.o Bank.o
RM=rm -rf
TARGET=Bank

$(TARGET): $(OBJS)
	$(CCLINK) $(CFLAGS) -o $(TARGET) $(OBJS)

Bank-Account.o: Bank-Account.cpp Bank-Account.h
Bank.o: Bank.cpp Bank.h Bank-Account.h Log-File.h
main.o: main.cpp Bank-Account.h Bank.h Log-File.h

clean:
	$(RM) $(OBJS) $(TARGET)