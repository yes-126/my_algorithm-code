CC      ?= gcc
CFLAGS  = -std=c17 -Wall -Wextra -O2 -Isrc
LIB     = src/sort.c src/insertionSort.c src/quickSort.c src/heapSort.c src/bench.c
HDRS    = $(wildcard src/*.h)

all: src/main.out

src/main.out: src/main.c $(LIB) $(HDRS)
	$(CC) $(CFLAGS) -o $@ src/main.c $(LIB)

tests/test_sort.out: tests/test_sort.c $(LIB) $(HDRS)
	$(CC) $(CFLAGS) -o $@ tests/test_sort.c $(LIB)

run: src/main.out
	./src/main.out

test: tests/test_sort.out
	./tests/test_sort.out

# 측정값을 CSV로 저장하고 그래프(SVG)를 그린다. python3만 있으면 된다
charts: src/main.out
	./src/main.out --csv > report/results.csv
	./src/main.out --pivot > report/pivot.csv
	python3 tools/plot.py

# CSV로 보고서에 붙일 마크다운 표를 만든다
tables:
	python3 tools/tables.py

clean:
	rm -f src/*.out tests/*.out
