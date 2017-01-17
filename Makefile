CC=gcc
MPICC=mpicc
CFLAGS=-Wall -ggdb -rdynamic -std=c99

SAMPLEDATA_SIZE="300"
SAMPLEDATA_LOCATION=res/sample
SAMPLEDATA_QSORT_LOCATION=res/qsort_sample
SAMPLEDATA_SSORT_LOCATION=res/ssort_sample

Q_OBJS=src/timer.o src/helpers.o src/quicksort.o
S_OBJS=src/samplesort.o src/timer.o src/helpers.o src/master.o src/client.o

all: sample-sort quick-sort

sample-sort: ${S_OBJS}
	${MPICC} ${CFLAGS} ${S_OBJS} -o bin/samplesort

quick-sort: ${Q_OBJS}
	${CC} ${CFLAGS} ${Q_OBJS} -o bin/qsort

src/timer.o: src/timer.c src/timer.h
	${CC} ${CFLAGS} -c src/timer.c -o src/timer.o

src/helpers.o: src/helpers.c src/helpers.h src/timer.o
	${CC} ${CFLAGS} -c src/helpers.c -o src/helpers.o

src/quicksort.o: src/quicksort.c src/quicksort.h
	${CC} ${CFLAGS} -c src/quicksort.c -o src/quicksort.o

src/samplesort.o: src/samplesort.c src/samplesort.h
	${MPICC} ${CFLAGS} -c src/samplesort.c -o src/samplesort.o

src/master.o: src/master.c src/master.h
	${MPICC} ${CFLAGS} -c src/master.c -o src/master.o

src/client.o: src/client.c src/client.h
	${MPICC} ${CFLAGS} -c src/client.c -o src/client.o

clean: clean-res clean-out

clean-res:
	rm -rf ${SAMPLEDATA_LOCATION}

clean-out:
	rm -rf bin/*
	rm -rf src/*.o

generator:
	${CC} ${CFLAGS} src/generator.c -o bin/generator

generate-input: generator
	bin/generator ${SAMPLEDATA_SIZE} ${SAMPLEDATA_LOCATION}

bench: generate-input bench-quicksort bench-samplesort

bench-quicksort: quick-sort
	echo "Quicksort:"
	time bin/qsort ${SAMPLEDATA_LOCATION} ${SAMPLEDATA_QSORT_LOCATION}

bench-samplesort: sample-sort
	echo "SampleSort:"
	time mpirun -np 8 bin/samplesort ${SAMPLEDATA_LOCATION} ${SAMPLEDATA_SSORT_LOCATION}
