CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra 
EXEC = project
FILES = main.c symtable.c scanner.c dynamic_string.c


all:
	$(CC) $(CFLAGS) -o $(EXEC) $(FILES) -lm

image:
	dot -Tpng symtableWriteGraph.dot -o symtableShowGraph.png

clear:
	rm project symtableShowGraph.png symtableWriteGraph.dot