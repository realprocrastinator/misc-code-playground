CC = gcc
CCFALG = -Wall -Werror -g -O0

include local-config.mk

OBJ_MAIN =  main.o
OBJ_LIB = mywrapper.o
OBJ_DLIB = libmywrapper.so
TARGET_NAME = dynprint


ifeq ($(USE_DYNLOAD), yes)
		EXTRA_LINK_FLAGS = -rdynamic -ldl
		EXTRA_COMPILE_FLAGS = -fpic -DCONFIG_USE_DYNLOAD
		OBJ_TARGETS = $(OBJ_MAIN) $(OBJ_LIB)
		TARGETS = $(OBJ_DLIB) $(TARGET_NAME)
else
		EXTRA_LINK_FLAGS =
		EXTRA_COMPILE_FLAGS = 
		OBJ_TARGETS = $(OBJ_MAIN) $(OBJ_LIB)
		TARGETS = $(TARGET_NAME)
endif 

all: $(TARGETS)

$(TARGET_NAME): $(OBJ_TARGETS)
		$(CC) $(CCFLAG) -o $@ $(OBJ_TARGETS) $(EXTRA_LINK_FLAGS)

$(OBJ_MAIN): main.c
		$(CC) $(CCFALG) -c $< -o $@ $(EXTRA_COMPILE_FLAGS)

$(OBJ_LIB): mywrapper.c
		$(CC) $(CCFALG) -c $< -o $@ $(EXTRA_COMPILE_FLAGS)

$(OBJ_DLIB): $(OBJ_LIB)
		$(CC) -shared $< -o $(OBJ_DLIB)

clean:
		rm $(TARGETS) $(OBJ_MAIN) $(OBJ_LIB)