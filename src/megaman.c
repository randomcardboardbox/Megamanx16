#include <stdlib.h>
#include "utils.h"
#include "megaman.h"
#include "globals.h"
#include "scroll.h"

struct MegamanStruct megaman_obj;

void load_megaman_spr_data(){
    char m_pal_filename[] = "megaman.pal";
    char m_spr_filename[] = "megaman.spr";
    char m_anim_filename[] = "megaman.anm";

    int *m_anim_addr = (int*) malloc(0x1000);
    char num_of_sprs = 0;
    char spr_ind = 0;
    
    _load_palette_from_file(m_pal_filename, 11, 0);
    _load_file_into_vram(m_spr_filename, 11, 0, 0x0000);
    _load_file_into_ram(m_anim_filename, 11, m_anim_addr);


    num_of_sprs = *m_anim_addr;
    spr_ind = alloc_sprites(num_of_sprs);

    megaman_obj.anim_addr = m_anim_addr;
    megaman_obj.num_of_sprs = *m_anim_addr;
    megaman_obj.spr_ind = spr_ind;

    megaman_obj.anim_timer = 0;
    megaman_obj.anim_index = 0;
    megaman_obj.frame = 0;
}

void check_collision(){
    int x_pos;
    int y_pos1;
    int y_pos2;
    int y_pos3;

    char check_1;
    char check_2;
    char check_3;

    int ver_y_pos_down = megaman_obj.y+11;
    int ver_y_pos_up = megaman_obj.y-11;
    int ver_x_pos1 = megaman_obj.x-6;//-megaman_obj.x_vel;
    int ver_x_pos2 = megaman_obj.x+6;//-megaman_obj.x_vel;

    char ver_check_1 = _check_collision_data(coll_data_addr, ver_x_pos1, ver_y_pos_down);
    char ver_check_2 = _check_collision_data(coll_data_addr, ver_x_pos2, ver_y_pos_down);

    char ver_check_3 = _check_collision_data(coll_data_addr, ver_x_pos1, ver_y_pos_up);
    char ver_check_4 = _check_collision_data(coll_data_addr, ver_x_pos2, ver_y_pos_up);

    char ver_check_5 = _check_collision_data(coll_data_addr, ver_x_pos1, megaman_obj.y);
    char ver_check_6 = _check_collision_data(coll_data_addr, ver_x_pos2, megaman_obj.y);

    if(ver_check_1 | ver_check_2){
        megaman_obj.y = ((megaman_obj.y-18) & 0b11110000)+21;
        megaman_obj.frac_y = 0;

        megaman_obj.y_vel = 0;
        megaman_obj.y_frac_vel = 0;

        megaman_obj.status = megaman_obj.status | 0b00000001;
    }
    else{
        megaman_obj.status = megaman_obj.status & 0b11111110;
    }
    
    if(ver_check_3 | ver_check_4){
        megaman_obj.y = ((megaman_obj.y) & 0b11110000)+16;
        megaman_obj.frac_y = 0;

        megaman_obj.y_vel = 0;
        megaman_obj.y_frac_vel = 0;
    }
    

    if(ver_check_5 | ver_check_6){
        megaman_obj.y = ((megaman_obj.y-24) & 0b11110000)+21;
        megaman_obj.frac_y = 0;

        megaman_obj.y_vel = 0;
        megaman_obj.y_frac_vel = 0;

        megaman_obj.status = megaman_obj.status | 0b00000001;
    }


    x_pos = megaman_obj.x;
    y_pos1 = megaman_obj.y-4;
    y_pos2 = megaman_obj.y-8;
    y_pos3 = megaman_obj.y+8;

    if(megaman_obj.x_vel < 127){
        x_pos += 8;
    }
    else{
        x_pos -= 8;
    }

    check_1 = _check_collision_data(coll_data_addr, x_pos, y_pos1);
    check_2 = _check_collision_data(coll_data_addr, x_pos, y_pos2);
    check_3 = _check_collision_data(coll_data_addr, x_pos, y_pos3);
    
    if(check_1 | check_2 | check_3){
        megaman_obj.x = ((megaman_obj.x) & 0b1111111111110000)+8;
        megaman_obj.frac_x = 0;
    }
}

void update_megaman(){
    int term_vel = 10;
    int old_pos_x = megaman_obj.x;
    int joystick = _get_joystick_state();

    megaman_obj.x_vel = 0;
    megaman_obj.x_frac_vel = 0;

    if(get_pressed(joystick, JOY_RIGHT)){
        megaman_obj.x_vel = 1;
        megaman_obj.x_frac_vel = 144;
        megaman_obj.status = megaman_obj.status | 0b00000010;
        megaman_obj.status = megaman_obj.status | 0b00000100;
    }
    else if(get_pressed(joystick, JOY_LEFT)){
        megaman_obj.x_vel = 254;
        megaman_obj.x_frac_vel = 112;
        megaman_obj.status = megaman_obj.status & 0b11111101;
        megaman_obj.status = megaman_obj.status | 0b00000100;
    }
    else{ megaman_obj.status = megaman_obj.status & 0b11111011; }
    
    if(get_pressed(joystick, JOY_A)){
        if((megaman_obj.status & 0b00000001)){
            megaman_obj.y_vel = -10;
            megaman_obj.y_frac_vel = 0;
        }
    }

    if((megaman_obj.status & 0b00000001) == 0){
        megaman_obj.y_vel += 1;
        if(megaman_obj.y_vel < 128 & megaman_obj.y_vel > term_vel){
            megaman_obj.y_vel = term_vel;
        }
    }

    _m_apply_velocity(&megaman_obj);
    check_collision();
    curr_room = megaman_obj.x>>8;
}

void animate_megaman(){
    char m_run_anim[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    char is_reverse = 0;
    int i=0;

    int x_pos = ((megaman_obj.x<<1) | (megaman_obj.frac_x>>7)) - 32;
    int y_pos = ((megaman_obj.y<<1) | (megaman_obj.frac_y>>7)) - 32;
   
    if((megaman_obj.status & 0b00000010) == 0){
        is_reverse = 1;
    }

    if(((megaman_obj.status & 0b00000001) > 0) & ((megaman_obj.status & 0b00000100) > 0)){
        play_anim(14, m_run_anim, &megaman_obj);
    }
    else{
        play_anim_frame(&megaman_obj);
        megaman_obj.frame = 0;
        megaman_obj.anim_timer = 0;
        megaman_obj.anim_index = 0;
    }
}
