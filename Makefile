CC=gcc
CFLAGS=-Wall

SAMPLEDATA_SIZE="10000000"
SAMPLEDATA_LOCATION=res/sample
SAMPLEDATA_QSORT_LOCATION=res/qsort_sample

Q_OBJS=src/timer.o src/quicksort.o

all: sample-sort quick-sort

sample-sort:

quick-sort: ${Q_OBJS}
	${CC} ${CFLAGS} ${Q_OBJS} -o bin/qsort

src/timer.o: src/timer.c
	${CC} ${CFLAGS} -c src/timer.c -o src/timer.o

src/quicksort.o: src/quicksort.c
	${CC} ${CFLAGS} -c src/quicksort.c -o src/quicksort.o

clean: clean-res clean-out

clean-res:
	rm ${SAMPLEDATA_LOCATION}

clean-out:
	rm bin/generator

generator:
	${CC} src/generator.c -o bin/generator

generate-input: generator
	bin/generator ${SAMPLEDATA_SIZE} ${SAMPLEDATA_LOCATION}

bench: generate-input bench-quicksort

bench-quicksort: quick-sort
	echo "Quicksort:"
	time bin/qsort ${SAMPLEDATA_LOCATION} ${SAMPLEDATA_QSORT_LOCATION}
