#include "utils.h"
#include "megaman.h"
#include "globals.h"
#include "object.h"
#include <stdlib.h>

struct ObjectStruct health_bar_obj;
struct ObjectStruct segment_objs[7];

void null_update(){

}

char flush_timer = 0;
char segment_frames[] = { 0,5,9,12,14 };
char initial_health = 28;
char started_flush = 0;
char seg_flush_time = 0;

void flush_red_health(){
    #define full_flush_time 20
    if(initial_health > megaman_obj.health && !started_flush){
        seg_flush_time = full_flush_time / (initial_health - megaman_obj.health);
        started_flush = 1;
        flush_timer = 15;
    }

    if(started_flush){
        if(flush_timer == 0){
            flush_timer = seg_flush_time;
            initial_health -= 1;

            if(initial_health == megaman_obj.health){
                started_flush = 0;
            }
        }

        flush_timer -= 1;
    }
}

void init_ui(){
    char m_hbar_filename[] = "hbar.spr";
    char m_hbar_pal_filename[] = "hbar.pal";
    char m_hbar_anim_filename[] = "hbar.anm";

    char m_seg_filename[] = "hbarseg.spr";
    char m_seg_pal_filename[] = "hbarseg.pal";
    char m_seg_anim_filename[] = "hbarseg.anm";

    int health_bar_spr_addr = 0x1B80;
    int health_bar_seg_spr_addr = 0x1C50;

    int *m_hbar_addr = (int*) malloc(0x560);
    int *m_hbarseg_addr = (int*) malloc(0x100);
    char spr_ind; 
    char num_of_sprs;

    char i = 0;

    segment_objs[0].spr_ind = alloc_sprites(1);
    segment_objs[1].spr_ind = alloc_sprites(1);
    segment_objs[2].spr_ind = alloc_sprites(1);
    segment_objs[3].spr_ind = alloc_sprites(1);
    segment_objs[4].spr_ind = alloc_sprites(1);
    segment_objs[5].spr_ind = alloc_sprites(1);
    segment_objs[6].spr_ind = alloc_sprites(1);

    _load_palette_from_file(m_hbar_pal_filename, 8, 1);
    _load_file_into_vram(m_hbar_filename, 8, health_bar_spr_addr>>12, health_bar_spr_addr<<4);
    _load_file_into_ram(m_hbar_anim_filename, 8, m_hbar_addr);

    _load_palette_from_file(m_seg_pal_filename, 11, 2);
    _load_file_into_vram(m_seg_filename, 11, health_bar_seg_spr_addr>>12, health_bar_seg_spr_addr<<4);
    _load_file_into_ram(m_seg_anim_filename, 11, m_hbarseg_addr);
    
    
    // set up health bar sprite
    num_of_sprs = *m_hbar_addr;

    spr_ind = alloc_sprites(num_of_sprs);

    health_bar_obj.spr_ind = spr_ind;
    health_bar_obj.anim_timer = 0;
    health_bar_obj.anim_index = 0;
    health_bar_obj.frame = 0;
    health_bar_obj.obj_type_ref = 1;
    health_bar_obj.status = health_bar_obj.status | 0b00000010;

    object_defs[1].anim_addr = m_hbar_addr;
    object_defs[1].num_of_sprs = *m_hbar_addr;
    object_defs[1].spr_addr = health_bar_spr_addr;
    object_defs[1].pal_off = 1;
    object_defs[1].update_ptr = null_update;
    object_defs[1].draw_ptr = null_update;
    object_defs[1].spr_width = 32;
    object_defs[1].spr_height = 32;
    object_defs[1].interaction_type = 0;

    health_bar_obj.x = 20;
    health_bar_obj.y = 20;
    play_obj_struct_anim_frame(&health_bar_obj, 1);

    //set up health bar segment sprs
    object_defs[2].anim_addr = m_hbarseg_addr;
    object_defs[2].num_of_sprs = 1;
    object_defs[2].spr_addr = health_bar_seg_spr_addr;
    object_defs[2].pal_off = 2;
    object_defs[2].update_ptr = null_update;
    object_defs[2].draw_ptr = null_update;
    object_defs[2].spr_width = 16;
    object_defs[2].spr_height = 16;
    object_defs[2].interaction_type = 0;


    for(i=0; i<7; i++){
        segment_objs[i].anim_timer = 0;
        segment_objs[i].anim_index = 0;
        segment_objs[i].frame = 11;
        segment_objs[i].obj_type_ref = 2;
        segment_objs[i].status = segment_objs[i].status | 0b00000010;
        segment_objs[i].x = 23;
        segment_objs[i].y = 61-(8*i);
        segment_objs[i].frac_y = 128;
        play_obj_struct_anim_frame(&segment_objs[i], 1);
    }
}

void update_ui(){
    char i;
    char no_of_segs = ((megaman_obj.health) / 4);
    char no_of_red_segs = ((initial_health) / 4);
    char half_seg_no = megaman_obj.health % 4;
    char half_red_seg_no = initial_health % 4;

    for (i=0; i<7; i++){
        segment_objs[i].frame = 14;
        if(i < no_of_red_segs){
            segment_objs[i].frame = 4;
        }

        if(i == no_of_red_segs & half_red_seg_no > 0){
            segment_objs[i].frame = segment_frames[5-half_red_seg_no]-1;
        }
    }
    
    for (i=0; i<no_of_segs; i++){
        segment_objs[i].frame = 0;
    }

    if(half_seg_no > 0){
        if(no_of_segs == no_of_red_segs){
            segment_objs[no_of_segs].frame = segment_frames[5-half_red_seg_no]-1-half_seg_no;
        }
        else{
            segment_objs[no_of_segs].frame = 4 - half_seg_no;
        }
    }

    for (i=0; i<7; i++){
        play_obj_struct_anim_frame(&segment_objs[i], 1);
    }

    flush_red_health();
}


void render_text(int text_ptr, char text_size, char spr_num, int x_pos, int y_pos){
    
}