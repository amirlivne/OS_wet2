# Makefile for the Bank program
CC = g++
CFLAGS =  -g -std=c++0x -Wall
LIBS = -lpthread -pthread
CCLINK = $(CC) $(LIBS)
OBJS = main.o bank.o account.o
RM = rm -f
# Creating the  executable
bank: $(OBJS)
	$(CCLINK) -o Bank $(OBJS)
# Creating the object files
account.0: account.h account.c
bank.o: bank.cpp bank.h account.h account.cpp
main.o: main.cpp bank.h account.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) $(OBJS) *~ "#"* core.*