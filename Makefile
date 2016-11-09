PROGRAM = ourspike
CC = g++
LD = g++

OFILES = main.o machine.o Instruction.o Register.o Stack.o loader.o
HFILES = main.h machine.h Instruction.h Register.h Stack.h loader.h
CFILES = main.cpp machine.cpp Instruction.cpp Register.cpp Stack.cpp loader.cpp

$(PROGRAM): $(OFILES)
	$(LD) $(OFILES) -o $(PROGRAM)

.c.o:
	$(CC) -c $<

clean:
	rm -f *.o $(PROGRAM)