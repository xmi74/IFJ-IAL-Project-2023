CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra 
EXEC = project
FILES = src/main.c src/symtable.c src/scanner.c src/dynamic_string.c src/error.c src/expr.c src/abstract_syntax_tree.c src/stack.c


all:
	$(CC) $(CFLAGS) -o $(EXEC) $(FILES) -lm

image:
	dot -Tpng symtableWriteGraph.dot -o symtableShowGraph.png

clear:
	@rm project symtableShowGraph.png symtableWriteGraph.dot
	@cd src && rm *.o
