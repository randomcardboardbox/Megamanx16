#include <stdlib.h>
#include <stdio.h>
#include <cbm.h>

#include "zsmplayer.h"

#include "globals.h"
#include "utils.h"
#include "megaman.h"
#include "title_seq.h"
#include "ui.h"
#include "scroll.h"

#include "object.h"
#include "gutslvl.h"

#define MUSNAM "guttheme.zsm"
#define MUSBANK 30
#define LFN 0
#define DEVICE 8
#define SA 2
#define MODE 0
#define LOADTO 0xA000 

void init_music_player(void){
    cbm_k_setnam(MUSNAM);
    cbm_k_setlfs(LFN, DEVICE, SA);
    RAM_BANK_SEL = MUSBANK;
    cbm_k_load(MODE, LOADTO);

    zsm_init();
}

void set_layer_config(void){
    _set_layer0_enable(1);
    _set_layer1_enable(1);
    _set_sprites_enable(1);

    VERA_dc_hscale = 128;
    VERA_dc_vscale = 128;

    L0_CONFIG = 0b01000001;
    L0_MAPBASE = (map_l0_vram_addr >> 9);
    L0_TILEBASE = 0b00000011 | ((tile_set_addr2>>1)&0b11111100);

    L0_HSCROLL_L = 0;
    L0_HSCROLL_H = 0;
    L0_VSCROLL_L = 0;
    L0_VSCROLL_H = 0;
    
    L1_CONFIG = 0b01000010;
    L1_MAPBASE = (map_l1_vram_addr >> 9);
    L1_TILEBASE = 0b00000011 | ((tile_set_addr>>1)&0b11111100);

    L1_HSCROLL_L = 0;
    L1_HSCROLL_H = 0;
    L1_VSCROLL_L = 0;
    L1_VSCROLL_H = 0;

    DC_BORDER = 0;
    CTRL = CTRL | 0b00000010;
    DC_HSTART = 0;
    DC_HSTOP = (495 >> 2);
}

void load_code_segments(){
    char megaman_code_fname[] = "megaman.prg.01";
    char level_code_fname[] = "megaman.prg.02";
    char title_seq_code_fname[] = "megaman.prg.03";

    RAM_BANK_SEL = 1;
    _load_file_into_ram(megaman_code_fname, 15, RAM_BANK_ADDR);

    RAM_BANK_SEL = 2;
    _load_file_into_ram(level_code_fname, 15, RAM_BANK_ADDR);

    RAM_BANK_SEL = 3;
    _load_file_into_ram(title_seq_code_fname, 15, RAM_BANK_ADDR);
}


void game_update(void) {
    int index = 0;
    char obj_ind;

    RAM_BANK_SEL = 1;
    load_megaman_spr_data();
    init_ui();

    RAM_BANK_SEL = object_code_block;
    g_init_lvl();

    load_map_data();

    for(index=0; index<16; index++){
        spawn_check(index);
    }
    for(index=0; index<32; index++){                        // transfer mapbase data into vram
        RAM_BANK_SEL = tile_set_ram_bank+0;
        _load_vert_map_sect(0, 64, 0, index, tile_map0_ram_addr, map_l0_vram_addr);
        RAM_BANK_SEL = tile_set_ram_bank+1;
        _load_vert_map_sect(0, 64, 0, index, tile_map1_ram_addr, map_l1_vram_addr);
    }

    megaman_obj.x = 100;
    megaman_obj.y = 100;
    megaman_obj.health = 28;

    zsm_startmusic(MUSBANK, LOADTO);
    
    while(1){
        RAM_BANK_SEL = 1;
        update_megaman();
        update_ui();
        RAM_BANK_SEL = object_code_block;
        _update_objects(objects, scroll_x, &dealloc_obj);

        RAM_BANK_SEL = object_code_block;
        level_update();
        // update_objects();
        
        calc_scroll();

        _wait_for_nmi();
        set_scroll();

        RAM_BANK_SEL = 1;
        animate_megaman();

        // zsm_play();

        draw_objects();
    }
} 


void main(void){
    init_music_player();
    _init_irq_handler();
    load_code_segments();

    RAM_BANK_SEL = 3;
    title_sequence();

    set_layer_config();

    game_update();
}