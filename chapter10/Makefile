src = $(wildcard *.c)
out = $(patsubst %.c,%,$(src))
network = $(wildcard ../utils/*.c)
networkheader = "../utils"
all: $(out)
$(out):%:%.c
	gcc $< $(network) -o $@ -g -I $(networkheader)
clean:
	-rm -rf $(out)
.PHONY: clean all