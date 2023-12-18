#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "gutslvl.h"
#include "met.h"
#include "object.h"

int *met_anim_addr;
int met_spr_addr = 0x18C0;

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
    _load_palette_from_file(&met_pal_fname, 7, 6);
    _load_file_into_vram(met_spr_fname, 7, met_spr_addr>>12, met_spr_addr<<4);
    _load_file_into_ram(met_anim_fname, 7, met_anim_addr);


    //import met data

    bullet_anim_addr = (int*) malloc(0x210);
    _load_palette_from_file(&bullet_pal_fname, 10, 7);
    _load_file_into_vram(bullet_spr_fname, 10, bullet_spr_addr>>12, bullet_spr_addr<<4);
    _load_file_into_ram(bullet_anim_fname, 10, bullet_anim_addr);
}

void g_init_obj_refs(void){

    //set met object information
    object_defs[4].anim_addr = met_anim_addr;
    object_defs[4].num_of_sprs = *met_anim_addr;
    object_defs[4].pal_off = 6;
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
    object_defs[5].pal_off = 7;
    object_defs[5].spr_addr = bullet_spr_addr;
    object_defs[5].update_ptr = bullet_update;
    object_defs[5].draw_ptr = bullet_draw;
    object_defs[5].spr_width = 8;
    object_defs[5].spr_height = 8;
    object_defs[5].width = 4;
    object_defs[5].height = 4;
    object_defs[4].interaction_type = 4;
}

void g_init_lvl(void){
    char pal_fname[] = "gstage.pal";
    char spr_fname[] = "gstagetiles.spr";
    char spr2_fname[] = "gstagetiles2.spr";
    char map_info_fname[] = "guti.stg";

    map_info_addr = malloc(0x100);

    _load_file_into_ram(map_info_fname, 8, map_info_addr);  // load map information
    _load_palette_from_file(pal_fname, 11, 4);
    _load_file_into_vram(spr_fname, 16, tile_set_addr>>8, tile_set_addr<<8);         // loading tilemap for level
    _load_file_into_vram(spr2_fname, 16, tile_set_addr2>>8, tile_set_addr2<<8);    

    g_init_load_assets();
    g_init_obj_refs();

    //set level background
    
}