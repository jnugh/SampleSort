SAMPLEDATA_SIZE="10000000"
SAMPLEDATA_LOCATION=res/sample

all: sample-sort quick-sort

sample-sort:

clean: clean-res clean-out

clean-res:
	rm ${SAMPLEDATA_LOCATION}

clean-out:
	rm bin/generator

generator:
	gcc src/generator.c -o bin/generator

generate-input: generator
	bin/generator ${SAMPLEDATA_SIZE} ${SAMPLEDATA_LOCATION}

bench: generate-input bench-quicksort

bench-quicksort: quick-sort
	echo "Quicksort:"
	time bin/qsort ${SAMPLEDATA_LOCATION}
