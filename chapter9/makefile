src = $(wildcard *.c)
target = $(patsubst %.c,%,$(src))
network = $(wildcard ../utils/*.c)
all: $(target)
$(target):%:%.c
	gcc $< $(network) -o $@ -g  -I $(network)
clean:
	-rm -rf $(target)
.PHONY: clean all