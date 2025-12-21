CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
PROGRAM = simple_shell

all: $(PROGRAM)

$(PROGRAM): simple_shell.c
	$(CC) $(CFLAGS) -o $(PROGRAM) simple_shell.c

clean:
	rm -f $(PROGRAM) *.o

run: $(PROGRAM)
	./$(PROGRAM)

debug: $(PROGRAM)
	gdb ./$(PROGRAM)

help:
	@echo "Targets:"
	@echo "  make      - Compile shell"
	@echo "  make run  - Run shell"
	@echo "  make clean- Remove binary"
	@echo "  make debug- Run under gdb debugger"

.PHONY: all clean run debug help
