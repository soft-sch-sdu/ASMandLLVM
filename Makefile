SRCS=$(wildcard *.cpp)
OBJS=$(SRCS: .cpp	= .o)

TIME = $(shell date)

2023spring: $(OBJS)
	@echo
	@echo "making..."
	clang++ $(CFLAGS)  -o $@ $^ $(LDFLAGS)
	@echo "$(TIME)"

test: clean 2023spring
	@echo
	@echo "testing..."
	./test.sh

clean:
	@echo "cleaning..."
	rm -f 2023spring *.o *~ tmp*

.PHONY: test clean
