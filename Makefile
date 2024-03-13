NIXRUN=nix-shell --run

main: main.c
	$(NIXRUN) "gcc -o main main.c -lraylib"

raylib:
	curl -L https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz | tar -xz

run: main
	./main
