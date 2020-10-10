#define STROBE_LOW		0x00
#define STROBE_HIGH		0x01

#define CTRL_UP_BUTTON		0x01
#define CTRL_DOWN_BUTTON	0x02
#define CTRL_LEFT_BUTTON	0x04
#define CTRL_RIGHT_BUTTON	0x08
#define CTRL_A_BUTTON		0x10
#define CTRL_B_BUTTON		0x20
#define CTRL_START_BUTTON	0x40
#define CTRL_SELECT_BUTTON	0x80

typedef struct __controller {
	__u8 buttons;
	__u8 strobe;
	__u8 last;
} _controller;

_controller c1;
