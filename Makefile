build:
	gcc src/main.c src/gameboy.c src/cpu.c src/mmu.c src/ppu.c -o src/emulator.exe

build-tests:
	gcc tests/boot_tests.c src/cpu.c src/mmu.c -o tests/boot_test.exe

clean:
	rm src/*.o src/*.exe src/*.out