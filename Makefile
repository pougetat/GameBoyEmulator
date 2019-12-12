build:
	gcc src/main.c src/cpu.c src/memory_map.c -o src/emulator.exe

build-test:
	gcc tests/boot_tests.c src/cpu.c src/memory_map.c -o test/boot_test.exe

clean:
	rm *.o *.exe *.out