/* added this file from my nes disassembler */

/* define addressing modes */
#define IMPLIED            0x1
#define ACCUMULATOR        0x2
#define IMMEDIATE          0x4
#define ZERO_PAGE          0x8
#define ZERO_PAGE_X       0x10
#define ZERO_PAGE_Y       0x20
#define INDIRECT_X        0x40
#define INDIRECT_Y        0x80
#define RELATIVE         0x100
#define ABSOLUTE         0x200
#define ABSOLUTE_X       0x400
#define ABSOLUTE_Y       0x800
#define INDIRECT        0x1000

#define NUM_OF_OPCODES	56

typedef struct __opcodes {
        __u8 num;              // hex number
        __u8 name_idx;         // index num of the name
        __u16 addr_mode;        // addressing mode
        __u8 cycles;           // how many cycles cmd uses
        __u8 cross_page;       // is there cycle penalty for cross-page usage
} _opcodes;

char opcode_names[NUM_OF_OPCODES][4] = {
        "ADC","AND","ASL","BCC","BCS","BEQ","BIT",
        "BMI","BNE","BPL","BRK","BVC","BVS","CLC",
        "CLD","CLI","CLV","CMP","CPX","CPY","DEC",
        "DEX","DEY","EOR","INC","INX","INY","JMP",
        "JSR","LDA","LDX","LDY","LSR","NOP","ORA",
        "PHA","PHP","PLA","PLP","ROL","ROR","RTI",
        "RTS","SBC","SEC","SED","SEI","STA","STX",
        "STY","TAX","TAY","TSX","TXA","TXS","TYA"
};

_opcodes opcodes[] = {
        {0x69,0,IMMEDIATE,2,0}, /* ADC */
        {0x65,0,ZERO_PAGE,3,0},
        {0x75,0,ZERO_PAGE_X,4,0},
        {0x6D,0,ABSOLUTE,4,0},
        {0x7D,0,ABSOLUTE_X,4,1},
        {0x79,0,ABSOLUTE_Y,4,1},
        {0x61,0,INDIRECT_X,6,0},
        {0x71,0,INDIRECT_Y,5,1},
        {0x29,1,IMMEDIATE,2,0}, /* AND */
        {0x25,1,ZERO_PAGE,3,0},
        {0x35,1,ZERO_PAGE_X,4,0},
        {0x2D,1,ABSOLUTE,4,0},
        {0x3D,1,ABSOLUTE_X,4,1},
        {0x39,1,ABSOLUTE_Y,4,1},
        {0x21,1,INDIRECT_X,6,0},
        {0x31,1,INDIRECT_Y,5,1},
        {0x0A,2,ACCUMULATOR,2,0}, /* ASL */
        {0x06,2,ZERO_PAGE,5,0},
        {0x16,2,ZERO_PAGE_X,6,0},
        {0x0E,2,ABSOLUTE,6,0},
        {0x1E,2,ABSOLUTE_X,7,0},
        {0x90,3,RELATIVE,2,1}, /* BCC */
        {0xB0,4,RELATIVE,2,1}, /* BCS */
        {0xF0,5,RELATIVE,2,1}, /* BEQ */
        {0x24,6,ZERO_PAGE,3,0},/* BIT */
        {0x2C,6,ABSOLUTE,4,0},
        {0x30,7,RELATIVE,2,1}, /* BMI */
        {0xD0,8,RELATIVE,2,1}, /* BNE */
        {0x10,9,RELATIVE,2,1}, /* BPL */
        {0x00,10,IMPLIED,7,0}, /* BRK */
        {0x50,11,RELATIVE,2,1}, /* BVC */
        {0x70,12,RELATIVE,2,1}, /* BVS */
        {0x18,13,IMPLIED,2,0}, /* CLC */
        {0xD8,14,IMPLIED,2,0}, /* CLD */
        {0x58,15,IMPLIED,2,0}, /* CLI */
        {0xB8,16,IMPLIED,2,0}, /* CLV */
        {0xC9,17,IMMEDIATE,2,0}, /* CMP */
        {0xC5,17,ZERO_PAGE,3,0},
        {0xD5,17,ZERO_PAGE_X,4,0},
        {0xCD,17,ABSOLUTE,4,0},
        {0xDD,17,ABSOLUTE_X,4,1},
        {0xD9,17,ABSOLUTE_Y,4,1},
        {0xC1,17,INDIRECT_X,6,0},
        {0xD1,17,INDIRECT_Y,5,1},
        {0xE0,18,IMMEDIATE,2,0}, /* CPX */
        {0xE4,18,ZERO_PAGE,3,0},
        {0xEC,18,ABSOLUTE,4,0},
        {0xC0,19,IMMEDIATE,2,0}, /* CPY */
        {0xC4,19,ZERO_PAGE,3,0},
        {0xCC,19,ABSOLUTE,4,0},
        {0xC6,20,ZERO_PAGE,5,0}, /* DEC */
        {0xD6,20,ZERO_PAGE_X,5,0},
        {0xCE,20,ABSOLUTE,5,0},
        {0xDE,20,ABSOLUTE_X,5,0},
        {0xCA,21,IMPLIED,2,0}, /* DEX */
        {0x88,22,IMPLIED,2,0}, /* DEY */
        {0x49,23,IMMEDIATE,2,0}, /* EOR */
        {0x45,23,ZERO_PAGE,3,0},
        {0x55,23,ZERO_PAGE_X,4,0},
        {0x4D,23,ABSOLUTE,4,0},
        {0x5D,23,ABSOLUTE_X,4,1},
        {0x59,23,ABSOLUTE_Y,4,1},
        {0x41,23,INDIRECT_X,6,0},
        {0x51,23,INDIRECT_Y,5,1},
        {0xE6,24,ZERO_PAGE,5,0}, /* INC */
        {0xF6,24,ZERO_PAGE_X,6,0},
        {0xEE,24,ABSOLUTE,6,0},
        {0xFE,24,ABSOLUTE_X,7,0},
        {0xE8,25,IMPLIED,2,0},  /* INX */
        {0xC8,26,IMPLIED,2,0}, /* INY */
        {0x4C,27,ABSOLUTE,3,0}, /* JMP */
        {0x6C,27,INDIRECT,5,0},
        {0x20,28,ABSOLUTE,6,0}, /* JSR */
        {0xA9,29,IMMEDIATE,2,0}, /* LDA */
        {0xA5,29,ZERO_PAGE,3,0},
        {0xB5,29,ZERO_PAGE_X,4,0},
        {0xAD,29,ABSOLUTE,4,0},
        {0xBD,29,ABSOLUTE_X,4,1},
        {0xB9,29,ABSOLUTE_Y,4,1},
        {0xA1,29,INDIRECT_X,6,0},
        {0xB1,29,INDIRECT_Y,5,1},
        {0xA2,30,IMMEDIATE,2,0}, /* LDX */
        {0xA6,30,ZERO_PAGE,3,0},
        {0xB6,30,ZERO_PAGE_Y,4,0},
        {0xAE,30,ABSOLUTE,4,0},
        {0xBE,30,ABSOLUTE_Y,4,1},
        {0xA0,31,IMMEDIATE,2,0}, /* LDY */
        {0xA4,31,ZERO_PAGE,3,0},
        {0xB4,31,ZERO_PAGE_X,4,0},
        {0xAC,31,ABSOLUTE,4,0},
        {0xBC,31,ABSOLUTE_X,4,1},
        {0x4A,32,ACCUMULATOR,2,0}, /* LSR */
        {0x46,32,ZERO_PAGE,5,0},
        {0x56,32,ZERO_PAGE_X,6,0},
        {0x4E,32,ABSOLUTE,6,0},
        {0x5E,32,ABSOLUTE_X,7,0},
        {0xEA,33,IMPLIED,2,0},   /* NOP */
        {0x09,34,IMMEDIATE,2,0}, /* ORA */
        {0x05,34,ZERO_PAGE,3,0},
        {0x15,34,ZERO_PAGE_X,4,0},
        {0x0D,34,ABSOLUTE,4,0},
        {0x1D,34,ABSOLUTE_X,4,1},
        {0x19,34,ABSOLUTE_Y,4,1},
        {0x01,34,INDIRECT_X,6,0},
        {0x11,34,INDIRECT_Y,5,1},
        {0x48,35,IMPLIED,3,0},   /* PHA */
        {0x08,36,IMPLIED,3,0}, /* PHP */
        {0x68,37,IMPLIED,4,0}, /* PLA */
        {0x28,38,IMPLIED,4,0}, /* PLP */
        {0x2A,39,ACCUMULATOR,2,0}, /* ROL */
        {0x26,39,ZERO_PAGE,5,0},
        {0x36,39,ZERO_PAGE_X,6,0},
        {0x2E,39,ABSOLUTE,6,0},
        {0x3E,39,ABSOLUTE_X,7,0},
        {0x6A,40,ACCUMULATOR,2,0}, /* ROR */
        {0x66,40,ZERO_PAGE,5,0},
        {0x76,40,ZERO_PAGE_X,6,0},
        {0x6E,40,ABSOLUTE,6,0},
        {0x7E,40,ABSOLUTE_X,7,0},
        {0x40,41,IMPLIED,6,0},  /* RTI */
        {0x60,42,IMPLIED,6,0}, /* RTS */
        {0xE9,43,IMMEDIATE,2,0}, /* SBC */
        {0xE5,43,ZERO_PAGE,3,0},
        {0xF5,43,ZERO_PAGE_X,4,0},
        {0xED,43,ABSOLUTE,4,0},
        {0xFD,43,ABSOLUTE_X,4,1},
        {0xF9,43,ABSOLUTE_Y,4,1},
        {0xE1,43,INDIRECT_X,6,0},
        {0xF1,43,INDIRECT_Y,5,1},
        {0x38,44,IMPLIED,2,0},  /* SEC */
        {0xF8,45,IMPLIED,2,0}, /* SED */
        {0x78,46,IMPLIED,2,0},  /* SEI */
        {0x85,47,ZERO_PAGE,3,0}, /* STA */
        {0x95,47,ZERO_PAGE_X,4,0},
        {0x8D,47,ABSOLUTE,4,0},
        {0x9D,47,ABSOLUTE_X,5,0},
        {0x99,47,ABSOLUTE_Y,5,0},
        {0x81,47,INDIRECT_X,6,0},
        {0x91,47,INDIRECT_Y,6,0},
        {0x86,48,ZERO_PAGE,3,0}, /* STX */
        {0x96,48,ZERO_PAGE_Y,4,0},
        {0x8E,48,ABSOLUTE,4,0},
        {0x84,49,ZERO_PAGE,3,0}, /* STY */
        {0x94,49,ZERO_PAGE_X,4,0},
        {0x8C,49,ABSOLUTE,4,0},
        {0xAA,50,IMPLIED,2,0}, /* TAX */
        {0xA8,51,IMPLIED,2,0}, /* TAY */
        {0xBA,52,IMPLIED,2,0}, /* TSX */
        {0x8A,53,IMPLIED,2,0}, /* TXA */
        {0x9A,54,IMPLIED,2,0}, /* TXS */
        {0x98,55,IMPLIED,2,0} /* TYA */
};

_opcodes lookup_table[256];
