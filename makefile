all: build

build:
	@gcc *.c *.h -pedantic -Wall -gnu=99
