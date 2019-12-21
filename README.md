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

=> Implement all instructions necessary for boot rom

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
	
=> Implement logo scroll

	New philosophy
		-> components can directly talk to each other, gameboy struct is still use as owner of all the component pointers, is responsible for stepping through the various components, and synchronizing their clocks.

	TASK : Implement LCD control register => 0xFF40
	
		Brief explanation :
		
			LCDC dictates bg + window display modes (on/off, where they read the data from)
			LCDC dictates sprite mode (on/off, where the sprites are located)
			Etc.
			
			https://gbdev.gg8.se/wiki/articles/LCDC
			https://nnarain.github.io/2016/09/09/Gameboy-LCD-Controller.html
	
		General design philosophy
		
			MMU should #define LCD control register as address 0xFF40 (included in header file so that it is available to PPU)
			The PPU never locks this register so no memory protection is needed
		
		To do items :
		
			0 - Implement lcdc_bg_enabled() -> set to 1 for logo scroll
			1 - Implement lcdc_sprite_enabled()
			2 - Implement lcdc_sprite_size_mode() -> _8_x_16 | _8_x_8
			3 - Implement lcdc_bg_tilemap_addr_mode() -> _0x9800_...
			4 - Implement lcdc_bg_win_tilemap_addr_mode() -> _0x….
			5 - Implement lcdc_win_enabled()
			6 - Implement lcdc_win_tilemap_addr_mode()
			7 - Implement lcdc_display_enabled()
			
			0 - Implement lcdc_set_bg_enabled()
			1 - Implement lcdc_set_sprite_enabled()
			2 - Implement lcdc_set_sprite_size_mode()
			3 - Implement lcdc_set_bg_tilemap_addr_mode()
			4 - Implement lcdc_set_bg_win_tilemap_addr_mode()
			5 - Implement lcdc_set_win_enabled()
			6 - Implement lcdc_set_win_tilemap_addr_mode()
			7 - Implement lcdc_set_display_enabled()
	
	TASK : implement LCD STAT register => 0xFF41
	
		Brief explanation :
	
			Indicates current status of the lcd controller module in the cpu (vblank, …) and interrupt selection according to this status (LYC = LY, OAM interrupt, V-blank, H-blank)
	
		General design philosophy
		
			No need to store the current mode in the struct, simply use the MMU to set and read this register.
			Need to implement memory protection MMU side for this specific address depending on the values
			
		To do items :
		
			0 - Implement stat_mode()
			1 - Implement stat_match_flag()
			2 - Implement stat_interrupt() -> returns and OR of 2 bit integers
			
			0 - Implement stat_set_mode() + reset_mode_clock()
			1 - Implement stat_set_match_flag()
			2 - Implement stat_set_interrupt()
	
	TASK : Implement SCY register / SCX register => 0xFF42 / 0xFF43
	
		Brief explanation :
		
			Indicates the position in the bg map that is to be displayed at the upper (SCY : 0) / left (SCX) LCD display position.
			(values : 0 -> 255)
			
	TASK : Implement LY register => 0xFF44
	
		Brief explanation :
			
			Indicates the vertical line to which the present data is transferred to the LCD driver (values : 0 -> 153). 
	
	TASK : integrate building of SDL2 into make process
	TASK : design GUI API that mmu will use to render
TASK : implement snapshot rendering of VRAM

# Learning resources

http://bgb.bircd.org/pandocs.htm
https://gbdev.gg8.se/wiki/articles/Pan_Docs#Cartridges
http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
https://github.com/gbdev/awesome-gbdev
http://www.emulatronia.com/doctec/consolas/gameboy/gameboy.txt

https://realboyemulator.wordpress.com/2013/01/03/a-look-at-the-game-boy-bootstrap-let-the-fun-begin/ => great explanation of the boot process

Once I am done I can look at the following emulators (NOT BEFORE) : 
https://github.com/binji/binjgb