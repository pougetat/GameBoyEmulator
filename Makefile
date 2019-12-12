build:
	gcc src/main.c src/cpu.c src/memory_map.c -o src/emulator.exe

build-tests:
	gcc tests/boot_tests.c src/cpu.c src/memory_map.c -o tests/boot_test.exe

clean:
	rm *.o *.exe *.out