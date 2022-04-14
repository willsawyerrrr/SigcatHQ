all: build

build:
	@gcc *.c *.h -pedantic -Wall -gnu=99 -L/local/courses/csse2310/lib -lcsse2310a3
