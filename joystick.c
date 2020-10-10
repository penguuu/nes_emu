#include "nes.h"
#include "joystick.h"

void joystick_init(){
	c1.buttons = 0;
	c1.strobe = STROBE_HIGH;
	c1.last = 0;
}

void joystick_set_strobe(__u8 val){
	if(val == STROBE_LOW)
		c1.strobe = STROBE_LOW;
	else 
		c1.strobe = STROBE_HIGH;

	c1.last = 0;
}

void joystick_press(__u8 button,__u8 ctrl_num){
	c1.buttons |= button;
}

void joystick_release(__u8 button, __u8 ctrl_num){
	c1.buttons &= (~button);
}

// return sequence: A,B,select,start,up,down,left,right
__u8 joystick_return_state(){
	if(c1.strobe == STROBE_HIGH)
		return (c1.buttons & CTRL_A_BUTTON);

	if(c1.strobe == STROBE_LOW){
		if(c1.last == 0){
			c1.last = CTRL_A_BUTTON;
			return ((c1.buttons & CTRL_A_BUTTON) ? 1 : 0);
		}
		else if(c1.last == CTRL_A_BUTTON){
			c1.last = CTRL_B_BUTTON;
			return ((c1.buttons & CTRL_B_BUTTON) ? 1 : 0);
		}
		else if(c1.last == CTRL_B_BUTTON){
			c1.last = CTRL_SELECT_BUTTON;
			return ((c1.buttons & CTRL_SELECT_BUTTON) ? 1 : 0);
		}
		else if(c1.last == CTRL_SELECT_BUTTON){
			c1.last = CTRL_START_BUTTON;
			return ((c1.buttons & CTRL_START_BUTTON) ? 1 : 0);
		}
		else if(c1.last == CTRL_START_BUTTON){
			c1.last = CTRL_UP_BUTTON;
			return ((c1.buttons & CTRL_UP_BUTTON) ? 1 : 0);
		}
		else if(c1.last == CTRL_UP_BUTTON){
			c1.last = CTRL_DOWN_BUTTON;
			return ((c1.buttons & CTRL_DOWN_BUTTON) ? 1 : 0);
		}
		else if(c1.last == CTRL_DOWN_BUTTON){	
			c1.last = CTRL_LEFT_BUTTON;
			return ((c1.buttons & CTRL_LEFT_BUTTON) ? 1 : 0);
		}	
		else if(c1.last == CTRL_LEFT_BUTTON){
			c1.last = CTRL_RIGHT_BUTTON;
			return ((c1.buttons & CTRL_RIGHT_BUTTON) ? 1 : 0);
		}
	}
}

