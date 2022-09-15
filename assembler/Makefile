assembler: main.o
	gcc -Wall -Wextra -std=c11 -pedantic main.o -o assembler

main.o: main.c
	gcc -Wall -Wextra -std=c11 -pedantic -c main.c -o main.o

.PHONY: clean
clean:
	rm assembler
	rm main.o
