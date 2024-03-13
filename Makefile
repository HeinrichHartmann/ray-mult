main:
	gcc -o main main.c -lraylib

run: main
	./main
