CC=gcc
CFLAGS=-I.
DEPS = SimpleProcessMonitor.h
OBJ = SimpleProcessMonitor.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

SimpleProcessMonitor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)