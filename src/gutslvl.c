#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "utils.h"
#include "gutslvl.h"
#include "met.h"

int *met_anim_addr;
int met_spr_addr = 0x1500;

int *bullet_anim_addr;
int bullet_spr_addr = 0x16D0;

void g_init_load_assets(void){
    //import met data
    char met_pal_fname[] = "met.pal";
    char met_spr_fname[] = "met.spr";
    char met_anim_fname[] = "met.anm";

    char bullet_pal_fname[] = "bullet.pal";
    char bullet_spr_fname[] = "bullet.spr";
    char bullet_anim_fname[] = "bullet.anm";

    met_anim_addr = (int*) malloc(0x500);
    _load_palette_from_file(&met_pal_fname, 7, 4);
    _load_file_into_vram(met_spr_fname, 7, met_spr_addr>>12, met_spr_addr<<4);
    _load_file_into_ram(met_anim_fname, 7, met_anim_addr);


    //import met data

    bullet_anim_addr = (int*) malloc(0x210);
    _load_palette_from_file(&bullet_pal_fname, 10, 5);
    _load_file_into_vram(bullet_spr_fname, 10, bullet_spr_addr>>12, bullet_spr_addr<<4);
    _load_file_into_ram(bullet_anim_fname, 10, bullet_anim_addr);
}

void g_init_obj_refs(void){
    //set met object information
    object_defs[1].anim_addr = met_anim_addr;
    object_defs[1].num_of_sprs = *met_anim_addr;
    object_defs[1].pal_off = 4;
    object_defs[1].spr_addr = met_spr_addr;
    object_defs[1].update_ptr = met_update;
    object_defs[1].draw_ptr = met_draw;
    object_defs[1].func_bank = 0;
    object_defs[1].width = 32;
    object_defs[1].height = 32;

    //set bullet object information
    object_defs[2].anim_addr = bullet_anim_addr;
    object_defs[2].num_of_sprs = *bullet_anim_addr;
    object_defs[2].pal_off = 5;
    object_defs[2].spr_addr = bullet_spr_addr;
    object_defs[2].update_ptr = bullet_update;
    object_defs[2].draw_ptr = bullet_draw;
    object_defs[2].func_bank = 0;
    object_defs[2].width = 8;
    object_defs[2].height = 8;
}

void g_init_lvl(void){
    char pal_fname[] = "gstage.pal";
    char spr_fname[] = "gstagetiles.spr";
    char map_info_fname[] = "guti.stg";

    map_info_addr = malloc(0x100);

    _load_file_into_ram(map_info_fname, 8, map_info_addr);  // load map information
    _load_palette_from_file(pal_fname, 11, 2);
    _load_file_into_vram(spr_fname, 16, tile_set_addr>>8, tile_set_addr<<8);         // loading tilemap for level

    g_init_load_assets();
    g_init_obj_refs();
}