build-linux:
	gcc-9 src/main.c src/gameboy.c src/cpu.c src/mmu.c src/ppu.c src/gui/gui.c -l SDL2 -I src/gui/include -o src/emulator.exe

build-macos:
	gcc-9 src/main.c src/gameboy.c src/cpu.c src/mmu.c src/ppu.c src/gui/gui.c -L src/gui/lib -l SDL2-2.0.0 -I src/gui/include -o src/emulator.exe

build-tests:
	gcc-9 tests/boot_tests.c src/gameboy.c src/cpu.c src/mmu.c src/ppu.c -o tests/boot_test.exe

clean:
	rm src/*.o src/*.exe src/*.out