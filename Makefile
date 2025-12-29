jdc-cli:
	gcc -c -w ./src/cli/main.c -o main.o
	gcc -c -w ./src/disassembler/*.c
	gcc -c -w ./src/decompiler/*.c
	gcc -c -w ./src/elf-handler/*.c
	gcc *.o -o ./bin/linux/x64/cli/jdc

debug-jdc-cli:
	gcc -g -O0 -c -w ./src/cli/main.c -o main.o
	gcc -g -O0 -c -w ./src/disassembler/*.c
	gcc -g -O0 -c -w ./src/decompiler/*.c
	gcc -g -O0 -c -w ./src/elf-handler/*.c
	gcc *.o -o ./bin/linux/x64/cli/debug-jdc

jdc-gui:
	g++ -c -w ./src/gui/*.cpp `../wxWidgets/buildgtk/wx-config --cxxflags --libs all`
	gcc -c -w ./src/disassembler/*.c
	gcc -c -w ./src/decompiler/*.c
	gcc -c -w ./src/elf-handler/*.c
	gcc -c -w ./src/file-handler/*.c
	g++ -o ./bin/linux/x64/gui/jdc *.o `../wxWidgets/buildgtk/wx-config --cxxflags --libs all`

debug-jdc-gui:
	g++ -g -O0 -c -w ./src/gui/*.cpp `../wxWidgets/buildgtk/wx-config --cxxflags --libs all`
	gcc -g -O0 -c -w ./src/disassembler/*.c
	gcc -g -O0 -c -w ./src/decompiler/*.c
	gcc -g -O0 -c -w ./src/elf-handler/*.c
	gcc -g -O0 -c -w ./src/file-handler/*.c
	g++ -o ./bin/linux/x64/gui/debug-jdc *.o `../wxWidgets/buildgtk/wx-config --cxxflags --libs all`

clean:
	rm *.o
