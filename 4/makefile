TARGETS = ask_4

all: $(TARGETS)

.PHONY: all clean

ask_4: ask_4.o
	gcc ask_4.o -o ask_4
ask_4.o: ask_4.c
	gcc ask_4.c -c -o ask_4.o

clean:
	rm ask_4.o ask_4