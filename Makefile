jdc: main.c ./src/disassembler/disassembler.c ./src/disassembler/disassembler.h
	gcc main.c -o main.o -c
	gcc -c -fdiagnostics-color=always ./src/disassembler/*.c 2>&1 | less -R
	gcc *.o -o ./bin/x64/jdc

debug-jdc: main.c ./src/disassembler/disassembler.c ./src/disassembler/disassembler.h
	gcc -g main.c -o main.o -c
	gcc -g -c -fdiagnostics-color=always ./src/disassembler/*.c 2>&1 | less -R
	gcc *.o -o ./bin/x64/debug-jdc

clean:
	rm *.o
	rm ./src/disassembler/*.gch
