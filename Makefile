BIN = run
CC = gcc
FLAGS = -Wall -Wextra -std=c11
INC = -I ./include
LOC_LIB = ./linux_x86_64/libGLEW.a -lglfw
SYS_LIB = -lGL -lm
SRC = main.c

all:
	@echo
	@echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	@echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~Building GNU/LINUX 64-bit~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	@echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	@echo
	${CC} ${FLAGS} -o ${BIN} ${SRC} ${INC} ${LOC_LIB} ${SYS_LIB}

run: all
	./run

