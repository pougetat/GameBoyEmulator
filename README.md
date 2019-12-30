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

=> Code cleanup

	TASK : [cpu] move macros into c file and change them into functions
	TASK : [cpu] make registers pointers to integers
	TASK : [Makefile] improvements can be made looking at standard makefiles
	TASK : make header files follow the same format (include order, external APIs prefixed but not internal APIs)

=> Elements necessary to execute the entire boot rom

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

	TASK : [test] verify BG palette setup
	TASK : [test] verify logo data properly loaded
	TASK : [test] verify loading of 8 additional bytes into VRAM
	TASK : [test] verify background tilemap setup
	
=> Elements of logo scrolling

	LCD CONTROL register => 0xFF40
		
			LCDC dictates bg + window display modes (on/off, where they read the data from)
			LCDC dictates sprite mode (on/off, where the sprites are located)
			Etc.
			
			https://gbdev.gg8.se/wiki/articles/LCDC
			https://nnarain.github.io/2016/09/09/Gameboy-LCD-Controller.html
	
	LCD STAT register => 0xFF41
		
			Indicates current status of the lcd controller module in the cpu (vblank, …) and interrupt selection according to this status (LYC = LY, OAM interrupt, V-blank, H-blank)
	
	TASK : Implement SCY register / SCX register => 0xFF42 / 0xFF43
		
			Indicates the position in the bg map that is to be displayed at the upper (SCY : 0) / left (SCX) LCD display position.
			(values : 0 -> 255)
			
	TASK : Implement LY register => 0xFF44
			
			Indicates the vertical line to which the present data is transferred to the LCD driver (values : 0 -> 153).

	TASK : verify VRAM data

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

# Learning resources

http://bgb.bircd.org/pandocs.htm
https://gbdev.gg8.se/wiki/articles/Pan_Docs#Cartridges
http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
https://github.com/gbdev/awesome-gbdev
http://www.emulatronia.com/doctec/consolas/gameboy/gameboy.txt

https://realboyemulator.wordpress.com/2013/01/03/a-look-at-the-game-boy-bootstrap-let-the-fun-begin/ => great explanation of the boot process

Once I am done I can look at the following emulators (NOT BEFORE) : 
https://github.com/binji/binjgb