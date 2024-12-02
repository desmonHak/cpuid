CC=gcc
CFLAGS1=-Wall -std=c11 -O3 
CFLAGS2=-Wall -std=c11 -O3 -D DISABLE_FUNCS_EXTERNS
EXTENSION=exe
CLEAR_COMMAND=del
CLEAR_DIR=rmdir /s /q

all: main.$(EXTENSION) pruebas.$(EXTENSION)  \
	cpuid.o pruebas2.$(EXTENSION) pruebas3.$(EXTENSION) pruebas4.$(EXTENSION) \
	pruebas5.$(EXTENSION)
	echo "compilando... con: $^"

main.$(EXTENSION): main.c
	$(CC) $(CFLAGS1) $^ -o $@

pruebas.$(EXTENSION): pruebas.c
	$(CC) $(CFLAGS1) $^ -o $@

pruebas2.$(EXTENSION): pruebas2.c
	$(CC) $(CFLAGS1) $^ -o $@

pruebas3.$(EXTENSION): pruebas3.c
	$(CC) $(CFLAGS1) $^ -o $@

pruebas4.$(EXTENSION): pruebas4.c
	$(CC) $(CFLAGS1) $^ -o $@

pruebas5.$(EXTENSION): pruebas5.c
	$(CC) $(CFLAGS1) $^ -o $@

cpuid.o: cpuid.c
	$(CC) $(CFLAGS2) -D_MSC_VER  $^ -c -o $@

build_python:
	python setup.py install

clear_python:
	$(CLEAR_DIR) build dist cpuid_native.egg-info __pycache__

clear:
	$(CLEAR_COMMAND) *.$(EXTENSION)
	$(CLEAR_COMMAND) cpuid.o