# Game boy emulator project

I will use this README to track my progress on my gameboy emulator project. The objectives of this project are the following :
- Learn about emulation by attempting to emulate a simple games system.
- Gain more experience in software design.
	- The code I will be writing will obviously be far from perfect. One idea is to check out existing emulators once I am finished with this project (not before) to try and understand how more experienced developers designed their systems and how I could have gained from their expertise.
- Gain more experience writing C code.
- Have fun.
	- It is very easy to feel paralyzed by the inadequacy of the codebase but I have to keep moving forward and complete my tasks. Otherwise, I will never have a working system. It won't be perfect and that's fine, let's have some fun.

More tasks will be added below as I move through the project and gain a better understanding of the system going from the general flow right down to the last detail.

# Milestone 1 : Scrolling the Nintendo logo (running the boot ROM)

## Work to be done

- TASK : Implement the necessary instructions to execute the boot ROM

	- ~~TASK : create a cpu.c file with the basic cpu struct.~~
	- ~~TASK : implement the fetch, decode, execute cycle with some simple instructions.~~
	- TASK : implement LD instructions
	- TASK : implement XOR instructions
	- TASK : implement BIT instructions
	- TASK : implement JR instructions
	- TASK : implement INC instructions
	- TASK : implement CALL instructions
	- TASK : implement CP instructions
	- TASK : implement JR instructions
	- TASK : implement DEC instructions
	- TASK : implement SUB instructions
	- TASK : implement PUSH instructions
	- TASK : implement RL instructions
	- TASK : implement RLA instructions
	- TASK : implement POP instructions
	- TASK : implement RET instructions
	- TASK : implement JR NZ instructions
	- TASK : implement ADD instructions

- TASK : implement basic PPU structure
	- TASK : setup ppu.c (modes, clock, basic render / scanline functions)
	- TASK : research graphical libraries to use (probably SDL).
	- TASK : implement rendering functions

- TASK : implement memory map structure
	- ~~TASK : load a boot ROM into the memory map.~~
	- TASK : rename memory_map to mmu.c
	- TASK : more abstraction over manipulating the memory to correctly implement valid VS invalid writes (VRAM cannot be modified during certain PPU modes, etc).

- TASK : logo scroll

- TASK : testing my boot sequence execution
	- ~~TASK : verify stack setup => add test~~
	- ~~TASK : verify VRAM is properly zeroed => add test~~
	- ~~TASK : verify audio setup => add test~~
	- ~~TASK : verify BG palette setup => add test~~
	- ~~TASK : verify logo data properly loaded => add test~~
	- ~~TASK : verify loading of 8 additional bytes into VRAM => add test~~
	- ~~TASK : verify background tilemap setup => add test~~

# Learning resources

http://bgb.bircd.org/pandocs.htm
https://gbdev.gg8.se/wiki/articles/Pan_Docs#Cartridges
http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
https://github.com/gbdev/awesome-gbdev
http://www.emulatronia.com/doctec/consolas/gameboy/gameboy.txt

https://realboyemulator.wordpress.com/2013/01/03/a-look-at-the-game-boy-bootstrap-let-the-fun-begin/ => great explanation of the boot process

Once I am done I can look at the following emulators (NOT BEFORE) : 
https://github.com/binji/binjgb