#include <stdlib.h>
#include "utils.h"
#include "megaman.h"
#include "globals.h"
#include "scroll.h"

//TODO: reimplement terminal velocity
//TODO: add gradual deceleration

int m_bullet_spr_addr = 0x1D00;
struct MegamanStruct megaman_obj;

void m_bullet_update(char obj_ind){
    _m_apply_velocity(&objects[obj_ind]);
}

void m_bullet_draw(char obj_ind){

}

void load_megaman_spr_data(){
    char m_pal_filename[] = "megaman.pal";
    char m_spr_filename[] = "megaman.spr";
    char m_anim_filename[] = "megaman.anm";

    char bullet_filename[] = "mbullet.spr";
    char bullet_pal_filename[] = "mbullet.pal";
    char bullet_anim_filename[] = "mbullet.anm";

    int *m_anim_addr = (int*) malloc(0x1C00);
    int *m_bullet_addr = (int*) malloc(0x100);
    char num_of_sprs = 0;
    char spr_ind = 0;
    
    _load_palette_from_file(m_pal_filename, 11, 0);
    _load_file_into_vram(m_spr_filename, 11, 0, 0x0000);
    _load_file_into_ram(m_anim_filename, 11, m_anim_addr);

    _load_palette_from_file(bullet_pal_filename, 11, 3);
    _load_file_into_vram(bullet_filename, 11, m_bullet_spr_addr>>12, m_bullet_spr_addr<<4);
    _load_file_into_ram(bullet_anim_filename, 11, m_bullet_addr);

    num_of_sprs = *m_anim_addr;
    spr_ind = alloc_sprites(num_of_sprs);

    megaman_obj.anim_addr = m_anim_addr;
    megaman_obj.num_of_sprs = *m_anim_addr;
    megaman_obj.spr_ind = spr_ind;

    megaman_obj.anim_timer = 0;
    megaman_obj.anim_index = 0;
    megaman_obj.frame = 0;

    //setting up megaman bullet object data
    object_defs[3].anim_addr = m_bullet_addr;
    object_defs[3].num_of_sprs = 1;
    object_defs[3].spr_addr = m_bullet_spr_addr;
    object_defs[3].pal_off = 3;
    object_defs[3].update_ptr = m_bullet_update;
    object_defs[3].draw_ptr = m_bullet_draw;
    object_defs[3].spr_width = 16;
    object_defs[3].spr_height = 16;
    object_defs[3].width = 8;
    object_defs[3].height = 8;
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

    char ver_check_1;
    char ver_check_2;
    char ver_check_3;
    char ver_check_4;
    char ver_check_5;
    char ver_check_6;

    ver_check_1 = _check_collision_data(lvl_data_bank, coll_data_addr, ver_x_pos1, ver_y_pos_down);
    ver_check_2 = _check_collision_data(lvl_data_bank, coll_data_addr, ver_x_pos2, ver_y_pos_down);

    ver_check_3 = _check_collision_data(lvl_data_bank, coll_data_addr, ver_x_pos1, ver_y_pos_up);
    ver_check_4 = _check_collision_data(lvl_data_bank, coll_data_addr, ver_x_pos2, ver_y_pos_up);

    ver_check_5 = _check_collision_data(lvl_data_bank, coll_data_addr, ver_x_pos1, megaman_obj.y);
    ver_check_6 = _check_collision_data(lvl_data_bank, coll_data_addr, ver_x_pos2, megaman_obj.y);

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
        if(megaman_obj.y_vel > 128){
            megaman_obj.y = ((megaman_obj.y) & 0b11110000)+16;
            megaman_obj.frac_y = 0;
            
            megaman_obj.y_vel = 0;
            megaman_obj.y_frac_vel = 0;
        }
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

    
    check_1 = _check_collision_data(lvl_data_bank, coll_data_addr, x_pos, y_pos1);
    check_2 = _check_collision_data(lvl_data_bank, coll_data_addr, x_pos, y_pos2);
    check_3 = _check_collision_data(lvl_data_bank, coll_data_addr, x_pos, y_pos3);
    
    if(check_1 | check_2 | check_3){
        megaman_obj.x = ((megaman_obj.x) & 0b1111111111110000)+8;
        megaman_obj.frac_x = 0;
    }
}

char is_holding_jump = 0;
char was_pressing_a = 0;
char jump_anim = 0;
char falling = 1;
char megaman_hurt = 0;
char megaman_invinc = 0;
char m_invnc_timer = 0;

void hurt_megaman(char dir, char health_decrease){
    if(!megaman_invinc){
        m_invnc_timer = 110;
        megaman_invinc = 1;
        megaman_hurt = 1;
        megaman_obj.anim_index = 0;
        megaman_obj.anim_timer = 0;
        megaman_obj.frame = 20;

        if(megaman_obj.health<health_decrease){
            megaman_obj.health = 0;
        }
        else{
            megaman_obj.health -= health_decrease;
        }

        is_holding_jump = 0;

        if(dir){
            megaman_obj.x_vel = -1;
            megaman_obj.status = megaman_obj.status | 0b00000010;
        }
        else{
            megaman_obj.x_vel = 0;
            megaman_obj.status = megaman_obj.status & 0b11111101;
        }
        megaman_obj.x_frac_vel = 180;

    }
}

char was_pressing_b = 0;
// char 
void shoot_check(int joystick){
    #define m_bullet_obj_ind 3

    char bullet_left_speed = 255-4;  // check game for actual value
    char bullet_right_speed = 4;

    if(get_pressed(joystick, JOY_B) && !was_pressing_b){
        char obj_ind = alloc_obj();

        was_pressing_b = 1;

        objects[obj_ind].obj_type_ref = m_bullet_obj_ind;
        objects[obj_ind].spr_ind = alloc_sprites(object_defs[m_bullet_obj_ind].num_of_sprs);
        objects[obj_ind].spawn_id = 0;
        objects[obj_ind].update_ptr = object_defs[m_bullet_obj_ind].update_ptr;
        objects[obj_ind].draw_ptr = object_defs[m_bullet_obj_ind].draw_ptr;

        objects[obj_ind].x = megaman_obj.x;
        objects[obj_ind].y = megaman_obj.y;

        if(megaman_obj.status & 0b00000010){
            objects[obj_ind].x_vel = bullet_right_speed;
        }
        else{
            objects[obj_ind].x_vel = bullet_left_speed;
        }
    }
    
    if(!get_pressed(joystick, JOY_B)){
        was_pressing_b = 0;
    }
}

void update_megaman(){
    int term_vel = 10;
    int old_pos_x = megaman_obj.x;
    int joystick = _get_joystick_state();
    char m_run_anim[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    char m_jump_anim[] = {1,15,16,17,18,19};
    char m_land_anim[] = {17,16,15,1};
    char m_hurt_anim[] = {20,  21,22,21,22,21,22,21,22,  23,24,0};

    char land_anim_check = _check_collision_data(lvl_data_bank, coll_data_addr, megaman_obj.x, megaman_obj.y+24);

    char is_reverse = 0;
    int i=0;
    char old_x_frac_vel = megaman_obj.x_frac_vel;


    megaman_obj.y = megaman_obj.y & 0b0000000011111111;
    if(megaman_obj.x < 16){
        megaman_obj.x = 16;
    }



    if(megaman_invinc){
        m_invnc_timer -= 1;
        if(m_invnc_timer == 0){
            megaman_invinc = 0;
        }
    }
    if(megaman_hurt){
        if(megaman_obj.anim_index == 11){
            megaman_hurt = 0;
        }
    }
    else{
        shoot_check(joystick);

        if(get_pressed(joystick, JOY_RIGHT)){
            if(megaman_obj.status & 0b00000001){
                megaman_obj.x_frac_vel += 13;
                if(megaman_obj.x_vel == 1 && megaman_obj.x_frac_vel > 72){
                    megaman_obj.x_frac_vel = 72;
                }
                if(old_x_frac_vel > megaman_obj.x_frac_vel){
                    megaman_obj.x_vel += 1;
                }
            }
            else{
                megaman_obj.x_vel = 1;
                megaman_obj.x_frac_vel = 72;
            }

            
            if((megaman_obj.status & 0b00000010) == 0){
                megaman_obj.x_vel = 0;
                megaman_obj.x_frac_vel = 0;
            }


            megaman_obj.status = megaman_obj.status | 0b00000010;
            megaman_obj.status = megaman_obj.status | 0b00000100;
        }
        else if(get_pressed(joystick, JOY_LEFT)){
            if(megaman_obj.status & 0b00000001){
                megaman_obj.x_frac_vel += 13;
                if(megaman_obj.x_vel == 254 && megaman_obj.x_frac_vel > 72){
                    megaman_obj.x_frac_vel = 72;
                }
                if(old_x_frac_vel > megaman_obj.x_frac_vel){
                    megaman_obj.x_vel -= 1;
                    megaman_obj.x_frac_vel = 0;
                }
            
            }
            else{
                megaman_obj.x_vel = 254;
                megaman_obj.x_frac_vel = 72;
            }

            if((megaman_obj.status & 0b00000010) || (megaman_obj.status & 0b00000100)==0){
                megaman_obj.x_vel = 255;
                megaman_obj.x_frac_vel = 0;
            }

            megaman_obj.status = megaman_obj.status & 0b11111101;
            megaman_obj.status = megaman_obj.status | 0b00000100;
        }
        else{ 
            megaman_obj.status = megaman_obj.status & 0b11111011; 
            megaman_obj.x_vel = 0;
            megaman_obj.x_frac_vel = 0;
        }
        
        if(get_pressed(joystick, JOY_A)){
            if((megaman_obj.status & 0b00000001) && was_pressing_a == 0){
                megaman_obj.anim_timer = 0;
                megaman_obj.anim_index = 0;
                jump_anim = 1;
                falling = 0;

                megaman_obj.y_vel = -6;
                megaman_obj.y_frac_vel = 200;
                is_holding_jump = 1;
            }
        }
    }

    was_pressing_a = get_pressed(joystick, JOY_A);
    if((megaman_obj.status & 0b00000001) == 0){
        char old_y_frac_vel = megaman_obj.y_frac_vel;
        megaman_obj.y_frac_vel += 70;
        if(old_y_frac_vel > megaman_obj.y_frac_vel){
            megaman_obj.y_vel += 1;
        }

        if(get_pressed(joystick, JOY_A) && !megaman_hurt && is_holding_jump){

            if(megaman_obj.y_vel < 128){
                is_holding_jump = 0;
            }
        }
        else{
            char old_y_frac_vel = megaman_obj.y_frac_vel;

            if(!is_holding_jump && megaman_obj.y_vel > 128){
                megaman_obj.y_frac_vel += 84;
                megaman_obj.y_vel += 1;
                if(old_y_frac_vel > megaman_obj.y_frac_vel){
                    megaman_obj.y_vel += 1;
                }
            }

            is_holding_jump = 0;

            if(megaman_obj.y_vel < 128 & megaman_obj.y_vel > term_vel){
                megaman_obj.y_vel = term_vel;
            }
        }
    }
    else{
        if(megaman_obj.y_vel < 0){
            is_holding_jump = 0;
        }
    }

    _m_apply_velocity(&megaman_obj);
    check_collision();
    curr_room = megaman_obj.x>>8;


    if(megaman_hurt){
        play_anim(12, m_hurt_anim, &megaman_obj);
    }
    else{
        if((megaman_obj.status & 0b00000010) == 0){
            is_reverse = 1;
        }
    

        if(((megaman_obj.status & 0b00000001) > 0) & ((megaman_obj.status & 0b00000100) > 0)){
            play_anim(14, m_run_anim, &megaman_obj);
            falling = 1;
        }
        else if((megaman_obj.status & 0b00000001) == 0){
            if(falling){
                megaman_obj.frame = 16;
                megaman_obj.anim_timer = 0;
                megaman_obj.anim_index = 0;
            }
            else{
                if(jump_anim == 1){
                    play_anim(6, m_jump_anim, &megaman_obj);
                    if(megaman_obj.anim_index == 5 || megaman_obj.y_vel < 128){
                        jump_anim = 0;
                        megaman_obj.anim_index = 0;
                        megaman_obj.anim_timer = 0;
                    }
                }
                else if(land_anim_check){
                    play_anim(4, m_land_anim, &megaman_obj);
                }
                else{
                    megaman_obj.frame = 19;
                    megaman_obj.anim_timer = 0;
                    megaman_obj.anim_index = 0;
                }
            }
        }
        else{
            megaman_obj.frame = 0;
            megaman_obj.anim_timer = 0;
            megaman_obj.anim_index = 0;
            jump_anim = 0;
            falling = 1;
        }
    }
}

void animate_megaman(){
    play_anim_frame(&megaman_obj);
}
