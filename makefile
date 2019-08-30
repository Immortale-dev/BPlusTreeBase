.PHONY: all generate_o generate_t

CC=g++
CFLAGS=-c -Wall -x c++
LDFLAGS=
SRCPATH:=src/
TESTPATH:=test/src/
SRCS:=$(wildcard $(SRCPATH)*.hpp)
SRCST:=$(wildcard $(TESTPATH)*.cpp)
OBJS:=$(SRCS:%.hpp=%.o)
OBJST:=$(SRCST:%.cpp=%.o)

all: generate_o generate_t

	
generate_o: ${OBJS} ${OBJST}

generate_t: 
	${CC} -Isrc -Itest/src -o test/test test/test.cpp 
	
	
	
%.test.o: %.test.cpp
	${CC} $(CFLAGS) $< -o $@ -Isrc

%.o: %.hpp
	${CC} ${CFLAGS} $< -o $@
	
