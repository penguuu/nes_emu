#include "nes.h"
#include "ppu.h"
#include "mem.h" 
#include "cartridge.h"
#include "joystick.h"

void usage(){
	printf("-h --help\t\tshow this help\n");
	printf("-d --debug\t\tturn on debug mode\n");
	printf("-r --rom <filename>\tload rom <filename>\n");
	printf("-p --print_header\tprint rom header and exit\n");
}

int main(int argc, char **argv){
	int opt = 0, c = 0;
	int print_header = 0;
	char rom_file[FILENAME_LEN+1];

	memset(&rom_file,0,FILENAME_LEN+1);

	while(1){
		static struct option long_options[] = {
			{"print_header",no_argument,0,'p'},
			{"rom",required_argument,0,'r'},
			{"help",no_argument,0,'h'},
			{0,0,0,0}
		};

		c = getopt_long(argc,argv,"pr:h",long_options,&opt);

		if(c == -1)
			break;

		switch(c){
			case 'p':
				print_header = 1;
				break;
			case 'r':
				printf("opt: %s\n",optarg);
				strncpy(rom_file,optarg,FILENAME_LEN);
				break;
			case 'h':
				usage();
				break;
			case '?':
				break;
			default:
				abort();
		}
	}

	open_debug_files();			

	if(rom_file[0] == 0){
		fprintf(stderr,"No rom file selected!\n");
		usage();
		return 0;
	}
	
	printf("rom_file: %s\n",rom_file);
	if(!load_cartridge_rom(&rom_file)){
		fprintf(stderr,"Unable to file %s\n",rom_file);
		return 0;
	}
	else {

		debug_print("title: %s\n",title);
		debug_print("init_sdl()\n");
		init_sdl();
		debug_print("ppu_init()\n");
		ppu_init();
		debug_print("cpu_start()\n");
		cpu_start();
		debug_print("ppu_start()\n");
		ppu_start();
		debug_print("joystick_init()\n");
		joystick_init();
		debug_print("run_nes()\n");
		run_nes();
	}
	unload_cartridge_rom();
	close_sdl();
	close_debug_files();
}

int run_nes(){
	int running = 1;
	SDL_Event event;

	while(running){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_KEYDOWN:  // key pressed
					switch(event.key.keysym.sym){
						case SDLK_LEFT:
							joystick_press(CTRL_LEFT_BUTTON,0);
							break;
						case SDLK_RIGHT:
							joystick_press(CTRL_RIGHT_BUTTON,0);
							break;
						case SDLK_UP:
							joystick_press(CTRL_UP_BUTTON,0);
							break;
						case SDLK_DOWN:
							joystick_press(CTRL_DOWN_BUTTON,0);
							break;
						case SDLK_LCTRL:
							joystick_press(CTRL_A_BUTTON,0);
							break;
						case SDLK_LALT:
							joystick_press(CTRL_B_BUTTON,0);
							break;
						case SDLK_z:
							joystick_press(CTRL_START_BUTTON,0);
							break;
						case SDLK_x:
							joystick_press(CTRL_SELECT_BUTTON,0);
							break;						
						case SDLK_q:
							running = 0;
							break;	
						default:
							break;
					}
					break;
				case SDL_KEYUP: // key released
					switch(event.key.keysym.sym){
						case SDLK_LEFT:
							joystick_release(CTRL_LEFT_BUTTON,0);
							break;
						case SDLK_RIGHT:
							joystick_release(CTRL_RIGHT_BUTTON,0);
							break;
						case SDLK_UP:
							joystick_release(CTRL_UP_BUTTON,0);
							break;
						case SDLK_DOWN:
							joystick_release(CTRL_DOWN_BUTTON,0);
							break;
						case SDLK_LCTRL:
							joystick_release(CTRL_A_BUTTON,0);
							break;
						case SDLK_LALT:
							joystick_release(CTRL_B_BUTTON,0);
							break;
						case SDLK_z:
							joystick_release(CTRL_START_BUTTON,0);
							break;
						case SDLK_x:
							joystick_release(CTRL_SELECT_BUTTON,0);
							break;		
						default:
							break;
					}
					break;				
				default:
					break;
			}
		}			
		run_ticks();
	}
}

int run_ticks(){
	__u16 ticks_ran;
	
	ticks_ran = (__u16) cpu_run_ticks(1);

#ifdef DEBUG
	debug_to_file(DEBUG_CPU,"CYCLES: %d, SCANLINE: %d\n",ppu.cycle_count,ppu.scanline);
#endif 
	ppu_tick(ticks_ran);
}
