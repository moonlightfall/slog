LOG_PATH = $(shell "pwd")/..

CC =cc

INCLUDE = -I../inc
LIB=-lpthread

OBJ += $(patsubst %.c, %.o, $(wildcard $(LOG_PATH)/src/*.c))

CFLAGS = -fPIC -O2 -g3 -Wall
TARGET = libslog.so
BUILD_OBJ = $(LOG_PATH)/build/out/*.o

all:$(OBJ)
	$(CC) $(BUILD_OBJ) -fPIC -shared -o $(TARGET) $(LIB)
	mv $(TARGET) out
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)
	mv $@ out
clean:
	rm -rf out/*
install:
	cp -f $(LOG_PATH)/build/out/$(TARGET) /usr/lib64 
