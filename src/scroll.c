#include <stdio.h>

#include "zsmplayer.h"

#include "object.h"
#include "globals.h"
#include "utils.h"
#include "megaman.h"
#include "gutslvl.h"

int old_scroll_block = 0;
int old_scroll_block_bg = 0;
int old_scroll_block_y = 0;
int scroll_speed = 8;
int megaman_frac_vel = 200; 


void load_new_room_ver(){
    int frame_index = 0;
    int index=0;
    int scroll_block;

    lvl_num += 1;
    tile_map[5] = 0x30+(lvl_num);
    tile_map2[5] = 0x30+(lvl_num);
    col_map[5] = 0x30+(lvl_num);

    load_map_data();
    curr_room = 0;

    for(index=0; index<16; index++){
        spawn_check(index);
    }
    for(index=0; index<32; index++){
        RAM_BANK_SEL = tile_set_ram_bank+0;
        _load_vert_map_sect(0, 64, 1, index, tile_map0_ram_addr, map_l0_vram_addr);
        RAM_BANK_SEL = tile_set_ram_bank+1;
        _load_vert_map_sect(0, 64, 1, index, tile_map1_ram_addr, map_l1_vram_addr);
    }

    scroll_y += scroll_speed;

    RAM_BANK_SEL = 2;
    level_transition_start(lvl_num);

    while((scroll_y%512) != 0){
        char old_frac_y = megaman_obj.frac_y;

        megaman_obj.frac_y += megaman_frac_vel;
        if(megaman_obj.frac_y < old_frac_y){
            megaman_obj.y+=1;
        } 

        _wait_for_nmi();
        //zsm_play();
        scroll_y += scroll_speed;
        L0_VSCROLL = scroll_y;
        L1_VSCROLL = scroll_y;
        RAM_BANK_SEL = 1;
        animate_megaman();

        RAM_BANK_SEL = 2;
        level_transition_update(lvl_num, frame_index);
        
        scroll_block = scroll_y >> 4;

        if(old_scroll_block_y != scroll_block & scroll_block != 0){
            for(index=0; index<32; index++){
                int load_pos = (scroll_block-1)*2;
                RAM_BANK_SEL = tile_set_ram_bank+0;
                _load_vert_map_sect(load_pos, 2, 0, index, tile_map0_ram_addr, map_l0_vram_addr);
                RAM_BANK_SEL = tile_set_ram_bank+1;
                _load_vert_map_sect(load_pos, 2, 0, index, tile_map1_ram_addr, map_l1_vram_addr);
            }
        }
        old_scroll_block_y = scroll_block;
    }

    megaman_obj.x = megaman_obj.x%256;
    megaman_obj.y = megaman_obj.y%256;
    scroll_x = 0;
    scroll_y = 0;
    L0_VSCROLL = scroll_y;
    L1_VSCROLL = scroll_y;

    L0_HSCROLL = scroll_x;
    L1_HSCROLL = scroll_x;
    
    for(index=0; index<32; index++){
        RAM_BANK_SEL = tile_set_ram_bank+0;
        _load_vert_map_sect(0, 2, 0, index, tile_map0_ram_addr, map_l0_vram_addr);
        RAM_BANK_SEL = tile_set_ram_bank+1;
        _load_vert_map_sect(0, 2, 0, index, tile_map1_ram_addr, map_l1_vram_addr);
    }

    RAM_BANK_SEL = 2;
    level_transition_end(lvl_num);
}


void load_new_room_hor(){
    int frame_index = 0;
    lvl_num += 1;

    load_map_data();
    // megaman_obj.x = 32;
    curr_room = 0;
    scroll_x += scroll_speed;

    RAM_BANK_SEL = 2;
    level_transition_start(lvl_num);

    while((scroll_x%512) != 0){
        int scroll_block;
        char old_frac_x = megaman_obj.frac_x;

        megaman_obj.frac_x += megaman_frac_vel;
        if(megaman_obj.frac_x < old_frac_x){
            megaman_obj.x+=1;
        } 

        _wait_for_nmi();
        //zsm_play();
        scroll_x += scroll_speed;
        L0_HSCROLL = scroll_x;
        L1_HSCROLL = scroll_x;

        RAM_BANK_SEL = 1;
        animate_megaman();

        RAM_BANK_SEL = 2;
        level_transition_update(lvl_num, frame_index);
        
        scroll_block = scroll_x >> 4;

        if(old_scroll_block != scroll_block){
            int load_pos = (scroll_block%32)-1;
            if(scroll_block%2 == 0){
                spawn_check(scroll_block/2);
            }
            RAM_BANK_SEL = tile_set_ram_bank+0;
            _load_vert_map_sect(0, 64, 0, load_pos, tile_map0_ram_addr, map_l0_vram_addr);
            RAM_BANK_SEL = tile_set_ram_bank+1;
            _load_vert_map_sect(0, 64, 0, load_pos, tile_map1_ram_addr, map_l1_vram_addr);
        }
        old_scroll_block = scroll_block;

        frame_index += 1;
    }

    // megaman_obj.x = 32;
    scroll_x = 0;
    megaman_obj.x = megaman_obj.x %256;
    megaman_obj.y = megaman_obj.y%256;
    RAM_BANK_SEL = tile_set_ram_bank+0;
    _load_vert_map_sect(0, 64, 0, 31, tile_map0_ram_addr, map_l0_vram_addr);
    RAM_BANK_SEL = tile_set_ram_bank+1;
    _load_vert_map_sect(0, 64, 0, 31, tile_map1_ram_addr, map_l1_vram_addr);

    RAM_BANK_SEL = 2;
    level_transition_end(lvl_num);
}

void check_room_transition(){
    // TODO:clear the object array when a new room is loaded

    int ent_dir = *(char *)(map_info_addr+(lvl_num*room_data_size)+7);
    int ext_dir = *(char *)(map_info_addr+(lvl_num*room_data_size)+8);

    if(ext_dir == 1){
        if(curr_room == last_room-1 & (megaman_obj.x%256)>224){
            clear_objs();
            load_new_room_hor();
        }
    }
    if(ext_dir == 4){
        if(curr_room == last_room-1 & (megaman_obj.y%256)>224){
            clear_objs();
            load_new_room_ver();
        }
    }
}

void set_scroll(){
    L0_HSCROLL = scroll_x_bg;
    L1_HSCROLL = scroll_x;

    L0_VSCROLL = scroll_y/bg_parallax_y;
    L1_VSCROLL = scroll_y;
}

void calc_scroll(){
    int scroll_block;
    int scroll_block_bg;
    int megaman_pos = (megaman_obj.x << 1) | megaman_obj.frac_x >> 7;
    int ram_sec_f = 0;
    int ram_sec_b = 0;
    int ram_sec_f_bg = 0;
    int ram_sec_b_bg = 0;

    last_room = *(char *)(map_info_addr+(lvl_num*room_data_size)+6);

    if(curr_room > 0  | ( curr_room == 0 & megaman_obj.x > 128)){
        scroll_x = ((curr_room << 9) | megaman_pos)-256;
    }
    else{ scroll_x = 0; }

    if((curr_room == last_room-1) & (megaman_obj.x%256) > 128){
        scroll_x = (curr_room << 9);
    }

    scroll_x_bg = scroll_x - (scroll_x/bg_parallax_x);

    scroll_block = scroll_x >> 4;
    scroll_block_bg = scroll_x_bg >> 4;
    ram_sec_f = (((scroll_x+496) / 2048))*2;
    ram_sec_b = ((scroll_x / 2048))*2;

    ram_sec_f_bg = (((scroll_x_bg+496) / 2048))*2;
    ram_sec_b_bg = ((scroll_x_bg / 2048))*2;


    if(old_scroll_block < scroll_block){
        if(scroll_block%2 == 0){
            spawn_check((scroll_block/2)+16);
        }
        RAM_BANK_SEL = ram_sec_f+tile_set_ram_bank+1;
        _load_vert_map_sect(0, 64, 0, (scroll_block+31)%128, tile_map1_ram_addr, map_l1_vram_addr);
    }
    if(old_scroll_block_bg < scroll_block_bg){
        RAM_BANK_SEL = ram_sec_f_bg+tile_set_ram_bank+0;
        _load_vert_map_sect(0, 64, 0, (scroll_block_bg+31)%128, tile_map0_ram_addr, map_l0_vram_addr);
    }

    if(old_scroll_block > scroll_block){
        if(scroll_block%2 == 0){ spawn_check(scroll_block/2); }
        RAM_BANK_SEL = ram_sec_b+tile_set_ram_bank+1;
        _load_vert_map_sect(0, 64, 0, scroll_block%128, tile_map1_ram_addr, map_l1_vram_addr);
    }
    if(old_scroll_block_bg > scroll_block_bg){
        RAM_BANK_SEL = ram_sec_b_bg+tile_set_ram_bank+0;
        _load_vert_map_sect(0, 64, 0, scroll_block_bg%128, tile_map0_ram_addr, map_l0_vram_addr);
    }

    old_scroll_block = scroll_block;
    old_scroll_block_bg = scroll_block_bg;

    check_room_transition();
}