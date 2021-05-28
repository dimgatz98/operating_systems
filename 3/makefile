TARGETS = ask3

# Targets to build when running `make`
all: $(TARGETS)

# Phony targets. Executed always.
.PHONY: all clean

# Targets to build `child` binary
ask3: ask3.o
	gcc ask3.o -o ask3 -lm
ask3.o: ask3.c
	gcc ask3.c -c -o ask3.o -lm

# Run `make clean` to delete binaries and object files.
clean:
	rm ask3.o ask3