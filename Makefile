.PHONY: build vendor

CC = gcc

build: vendor
	$(CC) -std=c99 \
		-I./vendor/raylib/src \
		main.c \
		-L./vendor/raylib/src \
		-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
		-Wall -Werror \
		-o main

vendor:
	$(MAKE) PLATFORM=PLATFORM_DESKTOP -C vendor/raylib/src