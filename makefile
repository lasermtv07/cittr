.PHONY: main

main: ws.c
	gcc ws.c -o ws -g -lsqlite3
