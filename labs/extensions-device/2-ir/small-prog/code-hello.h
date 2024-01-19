#include "libos-prog.h"
struct prog hello = {
    .name = "hello.bin",
    .nbytes = 200,
    .code = {
		0x78,	0x56,	0x34,	0x12,	0x14,	0x0,	0x0,	0x0,	
		0x0,	0x0,	0x30,	0x0,	0xb4,	0x0,	0x0,	0x0,	
		0x0,	0x0,	0x0,	0x0,	0x2,	0xd3,	0xa0,	0xe3,	
		0xe,	0x0,	0x0,	0xea,	0x0,	0x10,	0xd0,	0xe5,	
		0x0,	0x0,	0x51,	0xe3,	0x1e,	0xff,	0x2f,	0x1,	
		0x24,	0x20,	0x9f,	0xe5,	0x9a,	0xf,	0x7,	0xee,	
		0x64,	0x30,	0x92,	0xe5,	0x2,	0x0,	0x13,	0xe3,	
		0xfc,	0xff,	0xff,	0xa,	0x40,	0x10,	0x82,	0xe5,	
		0x9a,	0xf,	0x7,	0xee,	0x1,	0x10,	0xf0,	0xe5,	
		0x0,	0x0,	0x51,	0xe3,	0xf6,	0xff,	0xff,	0x1a,	
		0x1e,	0xff,	0x2f,	0xe1,	0x0,	0x50,	0x21,	0x20,	
		0x10,	0x40,	0x2d,	0xe9,	0x38,	0x0,	0x9f,	0xe5,	
		0xed,	0xff,	0xff,	0xeb,	0x34,	0x0,	0x9f,	0xe5,	
		0xeb,	0xff,	0xff,	0xeb,	0x9a,	0xf,	0x7,	0xee,	
		0x2c,	0x20,	0x9f,	0xe5,	0x64,	0x30,	0x92,	0xe5,	
		0x2,	0xc,	0x13,	0xe3,	0xfc,	0xff,	0xff,	0xa,	
		0x9a,	0xf,	0x7,	0xee,	0x1c,	0x30,	0x9f,	0xe5,	
		0x1c,	0x10,	0x9f,	0xe5,	0x1c,	0x20,	0x9f,	0xe5,	
		0x24,	0x10,	0x83,	0xe5,	0x1c,	0x20,	0x83,	0xe5,	
		0xfe,	0xff,	0xff,	0xea,	0xb4,	0x0,	0x30,	0x0,	
		0xbc,	0x0,	0x30,	0x0,	0x0,	0x50,	0x21,	0x20,	
		0x0,	0x0,	0x10,	0x20,	0x1,	0x0,	0x0,	0x5a,	
		0x20,	0x0,	0x0,	0x5a,	0x68,	0x65,	0x6c,	0x6c,	
		0x6f,	0xa,	0x0,	0x0,	0x44,	0x4f,	0x4e,	0x45,	
		0x21,	0x21,	0x21,	0xa,	0x0,	0x0,	0x0,	0x0,	    }
};
