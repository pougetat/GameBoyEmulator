# Game boy emulator project

I will use this README to track my progress on my gameboy emulator project. The objectives of this project are the following :
- Learn about emulation by attempting to emulate a simple games system.
- Gain more experience in software design.
	- The code I will be writing will obviously be far from perfect. One idea is to check out existing emulators once I am finished with this project (not before) to try and understand how more experienced developers designed their systems and how I could have gained from their expertise.
- Gain more experience writing C code.
- Have fun.
	- It is very easy to feel paralyzed by the inadequacy of the codebase but I have to keep moving forward and complete my tasks. Otherwise, I will never have a working system. It won't be perfect and that's fine, let's have some fun.

More tasks will be added below as I move through the project and gain a better understanding of the system going from the general flow right down to the last detail.

# Milestone 1 : Scrolling the Nintendo logo (running the boot ROM) [COMPLETE]

	This milestone has been reached. We can now scroll the Nintendo logo on to the screen. The tasks below are some suggested improvements for this stage.

## Work to be done

	TASK [cleanup]: move CPU macros into c file and change them into functions
	TASK [cleanup]: make CPU registers pointers to integers
	TASK [cleanup]: Improvements can be made to the makefile by looking at standard makefiles
	TASK [cleanup]: make header files follow the same format (include order, external APIs prefixed but not internal APIs)

	TASK [test]: verify VRAM data for logo scroll
		8000: 00000000000000000000000000000000
		8010: F000F000FC00FC00FC00FC00F300F300
		8020: 3C003C003C003C003C003C003C003C00
		8030: F000F000F000F00000000000F300F300
		8040: 000000000000000000000000CF00CF00
		8050: 000000000F000F003F003F000F000F00
		8060: 0000000000000000C000C0000F000F00
		8070: 000000000000000000000000F000F000
		8080: 000000000000000000000000F300F300
		8090: 000000000000000000000000C000C000
		80A0: 030003000300030003000300FF00FF00
		80B0: C000C000C000C000C000C000C300C300
		80C0: 000000000000000000000000FC00FC00
		80D0: F300F300F000F000F000F000F000F000
		80E0: 3C003C00FC00FC00FC00FC003C003C00
		80F0: F300F300F300F300F300F300F300F300
		8100: F300F300C300C300C300C300C300C300
		8110: CF00CF00CF00CF00CF00CF00CF00CF00
		8120: 3C003C003F003F003C003C000F000F00
		8130: 3C003C00FC00FC0000000000FC00FC00
		8140: FC00FC00F000F000F000F000F000F000
		8150: F300F300F300F300F300F300F000F000
		8160: C300C300C300C300C300C300FF00FF00
		8170: CF00CF00CF00CF00CF00CF00C300C300
		8180: 0F000F000F000F000F000F00FC00FC00

		8190: 3C004200B900A500B900A50042003C00 -> copyright logo

# Milestone 2 : Run through the entire boot process

	TASK : implement LD instructions
	TASK : implement XOR instructions
	TASK : implement BIT instructions
	TASK : implement JR instructions
	TASK : implement INC instructions
	TASK : implement CALL instructions
	TASK : implement CP instructions
	TASK : implement JR instructions
	TASK : implement DEC instructions
	TASK : implement SUB instructions
	TASK : implement PUSH instructions
	TASK : implement RL instructions
	TASK : implement RLA instructions
	TASK : implement POP instructions
	TASK : implement RET instructions
	TASK : implement JR NZ instructions
	TASK : implement ADD instructions

# Learning resources

http://bgb.bircd.org/pandocs.htm
https://gbdev.gg8.se/wiki/articles/Pan_Docs#Cartridges
http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
https://github.com/gbdev/awesome-gbdev
http://www.emulatronia.com/doctec/consolas/gameboy/gameboy.txt

- Good explanation of the LCDC register
https://gbdev.gg8.se/wiki/articles/LCDC
https://nnarain.github.io/2016/09/09/Gameboy-LCD-Controller.html

- Good explanation of the boot process
https://realboyemulator.wordpress.com/2013/01/03/a-look-at-the-game-boy-bootstrap-let-the-fun-begin/

Once I am done I can look at the following emulators (NOT BEFORE) : 
https://github.com/binji/binjgb