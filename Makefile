jdc: ./src/cli/main.c ./src/disassembler/disassembler.c ./src/disassembler/disassembler.h
	gcc ./src/cli/main.c -o main.o -c
	gcc -c -fdiagnostics-color=always ./src/disassembler/*.c 2>&1 | less -R
	gcc *.o -o ./bin/linux/jdc

debug-jdc: ./src/cli/main.c ./src/disassembler/disassembler.c ./src/disassembler/disassembler.h
	gcc -g ./src/cli/main.c -o main.o -c
	gcc -g -c -fdiagnostics-color=always ./src/disassembler/*.c 2>&1 | less -R
	gcc *.o -o ./bin/linux/debug-jdc

clean:
	rm *.o
	rm ./src/disassembler/*.gch
