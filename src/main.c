#include <stdlib.h>
#include <cbm.h>

#include "zsmplayer.h"

#include "globals.h"
#include "utils.h"
#include "megaman.h"
#include "scroll.h"

#include "object.h"
#include "gutslvl.h"

#define MUSNAM "guttheme.zsm"
#define MUSBANK 10
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

void main(void) {
    int index = 0;
    char obj_ind;
    
    init_music_player();

    _init_irq_handler();
    load_megaman_spr_data();

    set_layer_config();

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

    zsm_startmusic(MUSBANK, LOADTO);
    
    while(1){
        update_megaman();
        _update_objects(objects, scroll_x, &dealloc_obj);
        calc_scroll();

        _wait_for_nmi();
        //zsm_play();

        set_scroll();
        _draw_objects(&play_obj_anim_frame, objects);
        animate_megaman();
    }
}