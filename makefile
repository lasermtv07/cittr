.PHONY: main
main: src/dumbdb.c src/list.c src/parse.c src/ws.c
	gcc src/ws.c -o ws/ws -g
