jdc: main.c ../Disassembler/Source/disassembler.c ../Disassembler/Headers/disassembler.h
	gcc main.c -o main.o -c
	gcc -c -fdiagnostics-color=always ../Disassembler/Source/*.c 2>&1 | less -R
	gcc *.o -o jdc

debug-jdc: main.c ../Disassembler/Source/disassembler.c ../Disassembler/Headers/disassembler.h
	gcc -g main.c -o main.o -c
	gcc -g -c -fdiagnostics-color=always ../Disassembler/Source/*.c 2>&1 | less -R
	gcc *.o -o debug-jdc

clean:
	rm *.o
	rm ../Disassembler/Headers/*.gch
