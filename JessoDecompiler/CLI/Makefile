jdc: main.c ../Disassembler/disassembler.c ../Disassembler/disassembler.h
	gcc main.c -o main.o -c
	gcc -c -fdiagnostics-color=always ../Disassembler/*.c 2>&1 | less -R
	gcc *.o -o jdc

debug-jdc: main.c ../Disassembler/disassembler.c ../Disassembler/disassembler.h
	gcc -g main.c -o main.o -c
	gcc -g -c -fdiagnostics-color=always ../Disassembler/*.c 2>&1 | less -R
	gcc *.o -o debug-jdc

clean:
	rm *.o
	rm ../Disassembler/*.gch
