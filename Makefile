#
#SRCS=$(wildcard *.cpp)
#OBJS=$(SRCS: .cpp	= .o)
#
##LLVM_CONFIG := $(LLVM_HOME)/bin/llvm-config
##LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags)
##LLVM_LIBS := $(shell $(LLVM_CONFIG) --libs core support irreader)
#LLVM_CONFIG = $(LLVM_HOME)/bin/llvm-config
#LLVM_CXXFLAGS += $(shell $(LLVM_CONFIG) --cxxflags)
#LLVM_LDFLAGS += $(shell $(LLVM_CONFIG) --ldflags)
#LLVM_LIBS += $(shell $(LLVM_CONFIG) --libs  core support irreader)
#
#TIME = $(shell date)

#2023spring: $(OBJS)
#	@echo
#	@echo "making..."
#	clang++ -frtti $(LLVM_CXXFLAGS) $(LLVM_LIBS) $(LLVM_LDFLAGS)  -o $@ $^
#	@echo "$(TIME)"

test: #clean 2023spring
	@echo
	@echo "testing..."
	./test.sh

clean:
	@echo "cleaning..."
	rm -f 2023spring *.o *~ tmp*

.PHONY: test clean
