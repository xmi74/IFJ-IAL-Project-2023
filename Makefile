CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -pedantic
EXEC = project
FILES = src/main.c src/symtable.c src/scanner.c src/dynamic_string.c src/error.c src/expr.c src/abstract_syntax_tree.c src/stack.c

.PHONY: all image clear

all: $(EXEC)

$(EXEC): $(FILES)
	$(CC) $(CFLAGS) -o $@ $^ -lm

image:
	dot -Tpng symtableWriteGraph.dot -o symtableShowGraph.png

clear:
	rm -f $(EXEC) symtableShowGraph.png symtableWriteGraph.dot
	cd src && rm -f *.o