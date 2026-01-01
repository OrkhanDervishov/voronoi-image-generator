COMPILER = gcc
FLAGS = -Wall -Wextra
FILES = voronoi.c

build: $(FILES)
	$(COMPILER) $(FLAGS) -o $@ $^

run: build
	./build
