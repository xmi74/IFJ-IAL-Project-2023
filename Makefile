CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -pedantic
EXEC = parser
FILES = main.c error.c dynamic_string.c scanner.c stack.c symtable.c abstract_syntax_tree.c token_table.c expr.c parser.c code_gen.c

.PHONY: all image clear

all: $(EXEC)

$(EXEC): $(FILES)
	$(CC) $(CFLAGS) -o $@ $^ -lm

test: $(EXEC)
	@cd tests/ && ./test.sh