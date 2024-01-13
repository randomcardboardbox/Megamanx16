#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "gutslvl.h"
#include "met.h"
#include "object.h"


void g_level_transition_update(char level_num, int frame_index){}
void g_level_transition_start(char level_num){}
void g_level_transition_end(char level_num){}


#pragma code-name (push, "BANKRAM02")
int *met_anim_addr;
int met_spr_addr = 0x1940;

int *bullet_anim_addr;
int bullet_spr_addr = 0x1D20;

void g_init_load_assets(void){
    //import met data
    char met_pal_fname[] = "met.pal";
    char met_spr_fname[] = "met.spr";
    char met_anim_fname[] = "met.anm";

    char bullet_pal_fname[] = "bullet.pal";
    char bullet_spr_fname[] = "bullet.spr";
    char bullet_anim_fname[] = "bullet.anm";

    met_anim_addr = (int*) malloc(0x500);
    _load_palette_from_file(&met_pal_fname, 7, 9);
    _load_file_into_vram(met_spr_fname, 7, met_spr_addr>>12, met_spr_addr<<4);
    _load_file_into_ram(met_anim_fname, 7, met_anim_addr);


    //import met data

    bullet_anim_addr = (int*) malloc(0x210);
    _load_palette_from_file(&bullet_pal_fname, 10, 10);
    _load_file_into_vram(bullet_spr_fname, 10, bullet_spr_addr>>12, bullet_spr_addr<<4);
    _load_file_into_ram(bullet_anim_fname, 10, bullet_anim_addr);
}

void g_init_obj_refs(void){

    //set met object information
    object_defs[4].anim_addr = met_anim_addr;
    object_defs[4].num_of_sprs = *met_anim_addr;
    object_defs[4].pal_off = 9;
    object_defs[4].spr_addr = met_spr_addr;
    object_defs[4].update_ptr = met_update;
    object_defs[4].draw_ptr = met_draw;
    object_defs[4].spr_width = 32;
    object_defs[4].spr_height = 32;
    object_defs[4].width = 7;
    object_defs[4].height = 7;
    object_defs[4].interaction_type = 1;


    //set bullet object information
    object_defs[5].anim_addr = bullet_anim_addr;
    object_defs[5].num_of_sprs = *bullet_anim_addr;
    object_defs[5].pal_off = 10;
    object_defs[5].spr_addr = bullet_spr_addr;
    object_defs[5].update_ptr = bullet_update;
    object_defs[5].draw_ptr = bullet_draw;
    object_defs[5].spr_width = 8;
    object_defs[5].spr_height = 8;
    object_defs[5].width = 4;
    object_defs[5].height = 4;
    object_defs[4].interaction_type = 4;
}

void g_init_underground(void){
    char spr_fname[] = "gstagetilesu.spr";
    char map_info_fname[] = "guti.stg";

    _load_file_into_ram(map_info_fname, 8, map_info_addr);  // load map information
    _load_file_into_vram(spr_fname, 16, tile_set_addr>>8, tile_set_addr<<8);         // loading tilemap for level

    L0_CONFIG = 0b01000010;
    L0_MAPBASE = (map_l0_vram_addr >> 9);
    L0_TILEBASE = 0b00000011 | ((tile_set_addr>>1)&0b11111100);
}

void underground_pallete_shift(){
    char i;
    char pal_index = 4;
    char pal_timer = 10;

    char pal_fname[] = "gstageud4.pal";
    char pal_fnamef[] = "gstageu.pal";

    while(pal_index != 0 || pal_timer != 0){
        if(pal_timer == 0){
            pal_timer = 10;
            
            pal_fname[8] = 0x30+(pal_index);
            _load_palette_from_file(pal_fname, 14, 4);
            pal_index -= 1;
        }
        pal_timer -= 1;

        _wait_for_nmi();
    }

    _load_palette_from_file(pal_fnamef, 12, 4);
    while(pal_timer != 0){
        pal_timer -= 1;
    }
}

void palette_shift_overworld(){
    char i;
    char pal_index = 0;
    char pal_timer = 10;

    char pal_fname[] = "gstageb1.pal";

    while(pal_index != 4 || pal_timer != 0){
        if(pal_timer == 0){
            pal_timer = 10;
            
            pal_fname[7] = 0x30+(pal_index);
            _load_palette_from_file(pal_fname, 13, 4);
            pal_index += 1;
        }
        pal_timer -= 1;

        _wait_for_nmi();
    }
}

char did_blackout_cave_gfx = 0;
char did_load_cave_gfx = 0;

char pal_cycle_index = 0;
char pal_cycle_timer = 10;

void g_level_update(void){
    char cycle_arr[] = {1,2,3,2};
    char cycle_time_arr[] = {40,10,5,5};

    char megpal_fname[] = "megamanu.pal";
    char megpal_fname2[] = "megamanu2.pal";
    if(!did_blackout_cave_gfx && lvl_num == 1){
        char pal_fname[] = "dark.pal";

        palette_shift_overworld();
        _load_palette_from_file(megpal_fname2, 14, 0);
        _load_palette_from_file(pal_fname, 8, 4);
        g_init_underground();
        did_blackout_cave_gfx = 1;
    }

    if(!did_load_cave_gfx && lvl_num == 4){
        underground_pallete_shift();
        _load_palette_from_file(megpal_fname, 13, 0);
        megaman_obj.y_vel = 3;
        did_load_cave_gfx = 1;
    }


    if(lvl_num >= 4){
    // pallete cycling of lights
        if(pal_cycle_timer == 0){
            char cyclepal_fname[] = "lightosc1.pal";

            cyclepal_fname[8] = 0x30+(cycle_arr[pal_cycle_index]);
            _load_palette_from_file(cyclepal_fname, 14, 6);


            pal_cycle_timer = cycle_time_arr[pal_cycle_index];
            if(pal_cycle_index == 0){
                pal_cycle_timer = ((char)_get_random_int()) % 60;
                if(pal_cycle_timer < 4){
                    pal_cycle_timer = 4;
                }
            }
            pal_cycle_index += 1;
            if(pal_cycle_index > 3){
                pal_cycle_index = 0;
            }
        }
        pal_cycle_timer -= 1;
    }
}

void g_init_lvl(void){
    char pal_fname[] = "gstage.pal";
    char spr_fname[] = "gstagetiles.spr";
    char spr2_fname[] = "gstagetiles2.spr";
    char map_info_fname[] = "guti.stg";

    map_info_addr = malloc(0x100);

    _load_file_into_ram(map_info_fname, 8, map_info_addr);  // load map information
    _load_palette_from_file(pal_fname, 10, 4);
    _load_file_into_vram(spr_fname, 15, tile_set_addr>>8, tile_set_addr<<8);         // loading tilemap for level
    _load_file_into_vram(spr2_fname, 16, tile_set_addr2>>8, tile_set_addr2<<8);    

    g_init_load_assets();
    g_init_obj_refs();

    //set level background
    level_update = g_level_update;
    level_transition_update = g_level_transition_update;
    level_transition_start = g_level_transition_start;
    level_transition_end = g_level_transition_end;
}
#pragma code-name (pop)