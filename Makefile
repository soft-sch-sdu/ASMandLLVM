SRCS=$(wildcard *.cpp)
OBJS=$(SRCS: .cpp	= .o)

TIME = $(shell date)

2023spring: $(OBJS)
	clang++ $(CFLAGS) -Wall -o $@ $^ $(LDFLAGS)
	@echo "$(TIME)"

test:
	./test.sh

clean:
	rm -f 2023spring *.o *~ tmp*

.PHONY: test clean
