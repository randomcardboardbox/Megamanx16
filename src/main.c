#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "megaman.h"
#include "scroll.h"

void set_layer_config(void){
    _set_layer0_enable(1);
    _set_layer1_enable(1);
    _set_sprites_enable(1);

    VERA_dc_hscale = 128;
    VERA_dc_vscale = 128;

    L0_CONFIG = 0b01000010;
    L0_MAPBASE = (map_l0_vram_addr >> 9);
    L0_TILEBASE = 0b00000011 | ((tile_set_addr>>1)&0b11111100);

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
    DC_HSTOP = (496 >> 2);
}

void main(void) {
    char pal_fname[] = "gstage.pal";
    char spr_fname[] = "gstagetiles.spr";
    char map_info_fname[] = "guti.stg";
    int index = 0;

    map_info_addr = malloc(0x100);

    _load_file_into_ram(map_info_fname, 8, map_info_addr);  // load map information

    load_megaman_spr_data(&megaman_obj);

    _load_palette_from_file(pal_fname, 11, 2);
    _load_file_into_vram(spr_fname, 16, tile_set_addr>>8, tile_set_addr<<8);         // loading tilemap for level

    set_layer_config();                                     // set up for video
    load_map_data();                                        // load the mapbase data

    for(index=0; index<32; index++){                        // transfer mapbase data into vram
        RAM_BANK_SEL = 1;
        _load_vert_map_sect(0, 64, 0, index, tile_map0_ram_addr, map_l0_vram_addr);
        RAM_BANK_SEL = 2;
        _load_vert_map_sect(0, 64, 0, index, tile_map1_ram_addr, map_l1_vram_addr);
    }

    _init_irq_handler();

    megaman_obj.x = 100;
    megaman_obj.y = 100;

    while(1){
        update_megaman();
        //update other objects

        _wait_for_nmi();
        set_scroll();
        animate_megaman();
    }
}