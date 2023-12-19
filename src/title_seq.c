#include "title_seq.h"
#include "utils.h"
#include "object.h"
#include "globals.h"
#include "ui.h"
#include <stdlib.h>

#pragma code-name (push, "BANKRAM03")

struct ObjectStruct title1_obj;
struct ObjectStruct title2_obj;
struct ObjectStruct title3_obj;
struct ObjectStruct title4_obj;
struct ObjectStruct title5_obj;

int *title1_anim_addr;
int *title2_anim_addr;
int *title3_anim_addr;
int *title4_anim_addr;

char spr_ind1 = 0;
char spr_ind2 = 0;
char spr_ind3 = 0;
char spr_ind4 = 0;
char spr_ind5 = 0;

void title_load_sprites(){
    // TODO: remember to free animation ram addresses
    char title1_pal_filename[] = "title1.pal";
    char title4_pal_filename[] = "title4.pal";

    char title1_spr_filename[] = "title1.spr";
    char title1_anim_filename[] = "title1.anm";

    char title2_spr_filename[] = "title2.spr";
    char title2_anim_filename[] = "title2.anm";
    char title3_spr_filename[] = "title3.spr";
    char title3_anim_filename[] = "title3.anm";
    char title4_spr_filename[] = "title4.spr";
    char title4_anim_filename[] = "title4.anm";

    char font_spr_filename[] = "font.spr";
    char font_pal_filename[] = "font.pal";

    char num_of_sprs1 = 0;
    char num_of_sprs2 = 0;
    char num_of_sprs3 = 0;
    char num_of_sprs4 = 0;

    title1_anim_addr = malloc(0x200);
    title2_anim_addr = malloc(0x200);
    title3_anim_addr = malloc(0x200);
    title4_anim_addr = malloc(0x200);

    _load_palette_from_file(title1_pal_filename, 11, 0);
    _load_palette_from_file(title4_pal_filename, 11, 1);
    _load_file_into_vram(title1_spr_filename, 11, 0, 0x2800);

    _load_file_into_vram(title2_spr_filename, 11, 0, 0x4800);
    _load_file_into_vram(title3_spr_filename, 11, 0, 0xA800);
    _load_file_into_vram(title4_spr_filename, 11, 0, 0xE600);

    _load_file_into_ram(title1_anim_filename, 11, title1_anim_addr);
    _load_file_into_ram(title2_anim_filename, 11, title2_anim_addr);
    _load_file_into_ram(title3_anim_filename, 11, title3_anim_addr);
    _load_file_into_ram(title4_anim_filename, 11, title4_anim_addr);

    _load_file_into_vram(font_spr_filename, 9, 0, 0x0000);
    _load_palette_from_file(font_pal_filename, 9, 2);

    num_of_sprs1 = *title1_anim_addr;
    num_of_sprs2 = *title2_anim_addr;
    num_of_sprs3 = *title3_anim_addr;
    num_of_sprs4 = *title4_anim_addr;

    spr_ind3 = alloc_sprites(num_of_sprs3);
    spr_ind2 = alloc_sprites(num_of_sprs2);
    spr_ind1 = alloc_sprites(num_of_sprs1);
    spr_ind5 = alloc_sprites(num_of_sprs4);
    spr_ind4 = alloc_sprites(num_of_sprs4);

    // form title objects
    title1_obj.spr_ind = spr_ind1;
    title1_obj.anim_timer = 0;
    title1_obj.anim_index = 0;
    title1_obj.frame = 0;
    title1_obj.obj_type_ref = 1;
    title1_obj.status = title1_obj.status | 0b00000010;
    object_defs[1].anim_addr = title1_anim_addr;
    object_defs[1].num_of_sprs = *title1_anim_addr;
    object_defs[1].spr_addr = 0x0280;
    object_defs[1].pal_off = 0;
    object_defs[1].update_ptr = 0;
    object_defs[1].draw_ptr = 0;
    object_defs[1].spr_width = 32;
    object_defs[1].spr_height = 32;
    object_defs[1].interaction_type = 0;

    title2_obj.spr_ind = spr_ind2;
    title2_obj.anim_timer = 0;
    title2_obj.anim_index = 0;
    title2_obj.frame = 0;
    title2_obj.obj_type_ref = 2;
    title2_obj.status = title2_obj.status | 0b00000010;
    object_defs[2].anim_addr = title2_anim_addr;
    object_defs[2].num_of_sprs = *title2_anim_addr;
    object_defs[2].spr_addr = 0x0480;
    object_defs[2].pal_off = 0;
    object_defs[2].update_ptr = 0;
    object_defs[2].draw_ptr = 0;
    object_defs[2].spr_width = 32;
    object_defs[2].spr_height = 32;
    object_defs[2].interaction_type = 0;

    title3_obj.spr_ind = spr_ind3;
    title3_obj.anim_timer = 0;
    title3_obj.anim_index = 0;
    title3_obj.frame = 0;
    title3_obj.obj_type_ref = 3;
    title3_obj.status = title3_obj.status | 0b00000010;
    object_defs[3].anim_addr = title3_anim_addr;
    object_defs[3].num_of_sprs = *title3_anim_addr;
    object_defs[3].spr_addr = 0x0A80;
    object_defs[3].pal_off = 0;
    object_defs[3].update_ptr = 0;
    object_defs[3].draw_ptr = 0;
    object_defs[3].spr_width = 32;
    object_defs[3].spr_height = 32;
    object_defs[3].interaction_type = 0;

    title4_obj.spr_ind = spr_ind4;
    title4_obj.anim_timer = 0;
    title4_obj.anim_index = 0;
    title4_obj.frame = 0;
    title4_obj.obj_type_ref = 4;
    title4_obj.status = title4_obj.status | 0b00000010;
    title5_obj.spr_ind = spr_ind5;
    title5_obj.anim_timer = 0;
    title5_obj.anim_index = 0;
    title5_obj.frame = 0;
    title5_obj.obj_type_ref = 4;
    title5_obj.status = title5_obj.status & 0b11111101;
    object_defs[4].anim_addr = title4_anim_addr;
    object_defs[4].num_of_sprs = *title4_anim_addr;
    object_defs[4].spr_addr = 0x0E60;
    object_defs[4].pal_off = 1;
    object_defs[4].update_ptr = 0;
    object_defs[4].draw_ptr = 0;
    object_defs[4].spr_width = 32;
    object_defs[4].spr_height = 32;
    object_defs[4].interaction_type = 0;


    title1_obj.x = 105;//70;
    title1_obj.y = 50;

    title2_obj.x = 78;
    title2_obj.y = 70;

    title3_obj.x = 112;
    title3_obj.y = 110;

    title4_obj.x = 30;
    title4_obj.y = 10;

    title5_obj.x = 220;
    title5_obj.y = 10;
}

// title1_obj = 
void title_draw_sprites(){
    play_obj_struct_anim_frame(&title1_obj, 1);
    play_obj_struct_anim_frame(&title2_obj, 1);
    play_obj_struct_anim_frame(&title3_obj, 1);
    play_obj_struct_anim_frame(&title4_obj, 1);
    play_obj_struct_anim_frame(&title5_obj, 1);
}

void title_draw_text(){
    char start_text[] = "press start";
    char spr_ind = alloc_sprites(11);

    
    _render_text(2, 0x0000, &start_text, 200, 400, 11, 0xFC00+(spr_ind*8));
}

int title_tile_set_addr      = 0x088; // mulitply by 8 to get the actual addr
int title_tile_set_addr2      = 0x160;

void free_memory(){
    free(title1_anim_addr);
    free(title2_anim_addr);
    free(title3_anim_addr);
    free(title4_anim_addr);

    dealloc_sprites(spr_ind1);
    dealloc_sprites(spr_ind2);
    dealloc_sprites(spr_ind3);
    dealloc_sprites(spr_ind4);
    dealloc_sprites(spr_ind5);
}

void title_sequence(){
    title_load_sprites();
    
    _set_layer0_enable(0);
    _set_layer1_enable(0);
    _set_sprites_enable(1);

    L0_CONFIG = 0b00010001;
    L0_MAPBASE = (map_l0_vram_addr >> 9);
    L0_TILEBASE = 0b00000011 | ((title_tile_set_addr2>>1)&0b11111100);
    
    L1_CONFIG = 0b00010010;
    L1_MAPBASE = (map_l1_vram_addr >> 9);
    L1_TILEBASE = 0b00000011 | ((title_tile_set_addr>>1)&0b11111100);

    DC_BORDER = 0;
    CTRL = CTRL | 0b00000010;

    DC_HSTART = 0;
    DC_HSTOP = 480;

    title_draw_text();

    while(1){
        int joystick = _get_joystick_state();
        
        title_draw_sprites();
        _wait_for_nmi();

        if(get_pressed(joystick, JOY_SRT)){
            free_memory();

            break;
        }
    }
}

#pragma code-name (pop)