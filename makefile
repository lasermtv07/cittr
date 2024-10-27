.PHONY: main

main: ws.c
	gcc ws.c -o ws -ggdb -lsqlite3
