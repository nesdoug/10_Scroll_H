/*	example code for cc65, for NES
 *  scrolling, with vertical mirroring (horizontal arrangement)
 *	using neslib
 *	Doug Fraker 2018
 */	
 
 
 
#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "Sprites.h" // holds our metasprite data
#include "BG/N0.h" // backgrounds compressed as rle files
#include "BG/N1.h" 
 
 
 
#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
unsigned char sprid; // remember the index into the sprite buffer
unsigned char pad1;
unsigned int scroll_x;
unsigned int scroll_y;
unsigned char sprite_x = 0x80;
unsigned char sprite_y = 0x80;
unsigned char temp1;


#pragma bss-name(push, "BSS")





const unsigned char palette_bg[]={
0x0f, 0x00, 0x10, 0x30, // black, dk gray, lt gray, white
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 

const unsigned char palette_sp[]={
0x0f, 0x0f, 0x0f, 0x28, // black, black, yellow
0x0f, 0x0f, 0x0f, 0x15, // black, black, pink
0,0,0,0,
0,0,0,0
}; 


// PROTOTYPES
void draw_sprites(void);
void scrolling(void);	







	
	
void main (void) {
	
	ppu_off(); // screen off
	
	// load the palettes
	pal_bg(palette_bg);
	pal_spr(palette_sp);

	// use the second set of tiles for sprites
	// both bg and sprites are set to 0 by default
	bank_spr(1);

	vram_adr(NAMETABLE_A);
	// this sets a start position on the BG, top left of screen A
	
	vram_unrle(N0);
	// this unpacks a compressed full nametable
	
	vram_adr(NAMETABLE_B);
	// this sets a start position on the BG, top left of screen B
	
	vram_unrle(N1);
	// this unpacks a compressed full nametable

	ppu_on_all(); // turn on screen
	
	

	while (1){
		// infinite loop
		ppu_wait_nmi(); // wait till beginning of the frame
		// the sprites are pushed from a buffer to the OAM during nmi
		
		pad1 = pad_poll(0); // read the first controller
		
		scrolling();
		draw_sprites();
	}
}



void draw_sprites(void){
	// clear all sprites from sprite buffer
	oam_clear();

	// reset index into the sprite buffer
	sprid = 0;
	
	// draw 1 metasprite
	sprid = oam_meta_spr(sprite_x, sprite_y, sprid, YellowSpr);
	
	// draw the x and y as sprites
	sprid =  oam_spr(20,20,0xfe,1,sprid); // 0xfe = X
	temp1 = (scroll_x & 0xff) >> 4;
	// oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr,unsigned char sprid);
	sprid =  oam_spr(28,20,temp1,1,sprid);
	temp1 = (scroll_x & 0x0f);
	sprid =  oam_spr(36,20,temp1,1,sprid);
	
	sprid =  oam_spr(50,20,0xff,1,sprid); // 0xff = Y
	temp1 = (scroll_y & 0xff) >> 4;
	sprid =  oam_spr(58,20,temp1,1,sprid);
	temp1 = (scroll_y & 0x0f);
	sprid =  oam_spr(66,20,temp1,1,sprid);
}
	
	
	
void scrolling(void){
	if(pad1 & PAD_LEFT){
		scroll_x -= 1;
	}
	else if (pad1 & PAD_RIGHT){
		scroll_x += 1;
	}
	if(pad1 & PAD_UP){
		scroll_y = sub_scroll_y(1, scroll_y);
	}
	else if (pad1 & PAD_DOWN){
		scroll_y = add_scroll_y(1, scroll_y);
	}
	// scroll y is tricky, since values 240-255 are treated as -16 to -1
	// add_scroll_y,sub_scroll_y will adjust 16 if cross this range
	
	// set scroll
	set_scroll_x(scroll_x);
	set_scroll_y(scroll_y);
}	


