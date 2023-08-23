#ifndef X16_UTILS
#define X16_UTILS

#include "megaman.h"

// Assembly Function Definitions:
extern void _load_palette_from_file(int fname_ptr, char fname_size, char pal_off);
extern void _load_file_into_vram(int fname_ptr, char fname_size, char vram_bank, int vram_addr);
extern void _load_file_into_ram(int fname_ptr, char fname_size, int ram_addr);

extern void _set_sprite_attribute(char attr_num, int addr, char bpp_mode, int x, int y, char spr_attr, char size, char pal_off);
// spr_attr: col-mask:7-4, z-depth:3-2, v-flip, h-flip
extern void _clear_sprite_attribute(char attr_num);
extern void _offset_spr_position(char attr_num, int x_pos, int y_pos);

extern void _set_layer0_enable(char enable);
extern void _set_layer1_enable(char enable);
extern void _set_sprites_enable(char enable);

extern void _load_vert_map_sect(char start_pos, char num_of_tiles, char y_scoll, int pos, int ram_addr, long vram_addr);

extern char _check_collision_data(int col_data_addr, int x, int y);
extern void _transfer_ram_to_vram(int ram_src_addr, int size, char vram_bank, int vram_des_addr);
extern void _transfer_spr_attr_to_vram(char pal_off, int spr_addr_offset, int x_pos, int y_pos, int ram_src_addr, char vram_bank, int vram_des_addr);

extern void _wait_for_nmi(void);
extern void _init_irq_handler(void);

extern int _get_joystick_state(void);

extern void _force_halt(void);
extern void _play_obj_anim_frame(int x_off, int y_off, char pal_off, int spr_addr_off, int base_addr, char no_of_sprs, int VRAM_sprattr);
extern void _update_obj_pos(int x, int y, char pal_off, int base_addr, char no_of_sprs, int VRAM_sprattr);
extern void _play_obj_anim(void);

// Function Definitions:

char alloc_sprites(char size);
void dealloc_sprites(char index);

void play_anim_frame(struct MegamanStruct *obj);
void play_obj_anim_frame(char obj_ind);

void play_anim(char num_of_frames, char *anim_ram_addr, struct MegamanStruct *obj);
void play_obj_anim(char num_of_frames, char *anim_ram_addr, char obj_ind);

void load_map_data();

char get_pressed(int joystick, int button);
#define JOY_A           0b1000000000000000
#define JOY_X           0b0100000000000000
#define JOY_L           0b0010000000000000
#define JOY_R           0b0001000000000000

#define JOY_B           0b0000000010000000
#define JOY_Y           0b0000000001000000
#define JOY_SELECT      0b0000000000100000
#define JOY_START       0b0000000000010000
#define JOY_UP          0b0000000000001000
#define JOY_DOWN        0b0000000000000100
#define JOY_LEFT        0b0000000000000010
#define JOY_RIGHT       0b0000000000000001

// X16 Address Defintions:
#define VERA_dc_hscale (*(char *)0x9F2A)
#define VERA_dc_vscale (*(char *)0x9F2B)

#define L0_CONFIG (*(char *)0x9F2D)
#define L0_MAPBASE (*(char *)0x9F2E)
#define L0_TILEBASE (*(char *)0x9F2F)

#define L0_HSCROLL_L (*(char *)0x9F30)
#define L0_HSCROLL_H (*(char *)0x9F31)
#define L0_VSCROLL_L (*(char *)0x9F32)
#define L0_VSCROLL_H (*(char *)0x9F33)

#define L1_CONFIG (*(char *)0x9F34)
#define L1_MAPBASE (*(char *)0x9F35)
#define L1_TILEBASE (*(char *)0x9F36)

#define L1_HSCROLL_L (*(char *)0x9F37)
#define L1_HSCROLL_H (*(char *)0x9F38)
#define L1_VSCROLL_L (*(char *)0x9F39)
#define L1_VSCROLL_H (*(char *)0x9F3A)

#define CTRL (*(char *)0x9F25)

#define DC_HSTART (*(char *)0x9F29)
#define DC_HSTOP (*(char *)0x9F2A)
#define DC_VSTART (*(char *)0x9F2B)
#define DC_VSTOP (*(char *)0x9F2C)

#define DC_HSCALE (*(char *)0x9F2A)
#define DC_VSCALE (*(char *)0x9F2B)
#define DC_BORDER (*(char *)0x9F2C)

#define VRAM_sprattr (*(char *)0x1FC00)
#define VRAM_palette (*(char *)0x1FA00)

#define L0_HSCROLL (*(int *)0x9F30)
#define L0_VSCROLL (*(int *)0x9F32)
#define L1_HSCROLL (*(int *)0x9F37)
#define L1_VSCROLL (*(int *)0x9F39)

#define RAM_BANK_SEL (*(char *)0x0000)
#define ROM_BANK_SEL (*(char *)0x0001)

//global variables
// extern int scroll_x;

#endif //X16_UTILS