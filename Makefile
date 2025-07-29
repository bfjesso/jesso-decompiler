jdc: ./src/cli/main.c ./src/disassembler/disassembler.c ./src/decompiler/decompiler.c ./src/elf-handler/elfHandler.c
	gcc -c -w ./src/cli/main.c -o main.o
	gcc -c -w ./src/disassembler/*.c
	gcc -c -w ./src/decompiler/*.c
	gcc -c -w ./src/elf-handler/*.c
	gcc *.o -o ./bin/linux/x64/jdc

debug-jdc: ./src/cli/main.c ./src/disassembler/disassembler.c ./src/decompiler/decompiler.c ./src/elf-handler/elfHandler.c
	gcc -g -O0 -c -w ./src/cli/main.c -o main.o
	gcc -g -O0 -c -w ./src/disassembler/*.c
	gcc -g -O0 -c -w ./src/decompiler/*.c
	gcc -g -O0 -c -w ./src/elf-handler/*.c
	gcc *.o -o ./bin/linux/x64/debug-jdc

clean:
	rm *.o
	rm ./src/disassembler/*.gch
