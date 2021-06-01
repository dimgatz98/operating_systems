TARGETS = ask_4 server

all: $(TARGETS)

.PHONY: all clean

ask_4: ask_4.c
	gcc ask_4.c -o ask_4

server: server.c
	gcc server.c -o server

clean:
	rm ask_4 server