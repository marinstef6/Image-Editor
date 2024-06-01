# Marin Stefania, 311CA

# compiler setup
build: image_editor

image_editor: image_editor.c
	gcc -Wall -Wextra -std=c99 -o image_editor image_editor.c -lm

pack:
	zip -FSr 311CA_Marin_Stefania_Tema3.zip README Makefile *.c *.h

clean:
	rm -f *.o
	rm -f image_editor

.PHONY: pack clean