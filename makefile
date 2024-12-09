Assembler:
	gcc -Wall -Wextra -pedantic -o debug.out -I include/ src/*.c main.c

Assembler-Debug:
	gcc -g -Wall -Wextra -pedantic -o debug.out -I include/ src/*.c main.c