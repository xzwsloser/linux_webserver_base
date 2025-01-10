src = $(wildcard *.cc)
out = $(patsubst %.cc,%,$(src))
network = ../utils
timer = ./Timer/inc
CC = g++
networkimpl = $(wildcard ../utils/*.cc)
timerimpl = $(wildcard ./Timer/src/*.cc)

all: $(out)
$(out):%:%.cc
	$(CC) $< $(networkimpl) $(timerimpl) -o $@ -I $(network)  -I $(timer) -g 

clean:
	-rm -rf $(out)
.PHONY: clean all