#comment
CC=mpiCC
#CFLAGS=-DDEBUG -O0123
CFLAGS=-g -DDEBUG
LDLIBS=
INCLUDE=-I. -I./include
OBJS=command.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(LDLIBS) $(OBJS) main.cpp -o terminal
	$(CC) $(CFLAGS) $(INCLUDE) $(LDLIBS) car.cpp -o car
	$(CC) $(CFLAGS) $(INCLUDE) $(LDLIBS) ora.cpp -o ora
	$(CC) $(CFLAGS) $(INCLUDE) $(LDLIBS) parking.cpp -o main
#para cualquier .o que falte, busco sus cpp
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
