CC=gcc
CFLAGS1=-Wall -std=c11 -O3 
CFLAGS2=-Wall -std=c11 -O3 -D DISABLE_FUNCS_EXTERNS
EXTENSION=exe
CLEAR_COMMAND=del

main:
	$(CC) $(CFLAGS1) main.c -o main.$(EXTENSION)

cpuid.o:
	$(CC) $(CFLAGS2) cpuid.c -c -o cpuid.o 

clear:
	$(CLEAR_COMMAND) main.$(EXTENSION)
	$(CLEAR_COMMAND) cpuid.o 