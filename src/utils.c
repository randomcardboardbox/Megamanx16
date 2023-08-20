#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "utils.h"
#include "object.h"

char spr_alloc_table[128] = {0};

char alloc_sprites(char size){
    int i=0;
    int j=0;
    int init_ind=0;

    for (i=0; i<(127-size); i++){
        if(spr_alloc_table[i] != 0){
            init_ind = i+1;
        }
        if((i-init_ind)==(size)){
            spr_alloc_table[init_ind] = size;
            for (j=1; j<size; j++){
                spr_alloc_table[init_ind+j] = 1;
            }
            return(init_ind);
        }
    }

    spr_alloc_table[127-size] = size;
    for (j=1; j<size; j++){
        spr_alloc_table[(127-size)+j] = 1;
    }
    return(127-size);
}

void dealloc_sprites(char index){
    int i=0;
    int size=spr_alloc_table[index];
    if(size == 0){
        size = 1;
    }

    for (i=0; i<size; i++){
        spr_alloc_table[index+i] = 0;
        _clear_sprite_attribute(index+i);
    }
}


void play_anim_frame(struct MegamanStruct *obj){
    char is_reverse = 0;
    int i=0;
    int x = ((((obj->x<<1) | (obj->frac_x>>7)) - 32)-scroll_x)&0b0000001111111111;
    int y = ((((obj->y<<1) | (obj->frac_y>>7)) - 32)-scroll_y)&0b0000000111111111;

    int base_addr;

    if(!(obj->status & 0b00000010)){ is_reverse = 1; }
    base_addr = (obj->anim_addr+3)+(is_reverse*8*obj->num_of_sprs)+(2+16*obj->num_of_sprs)*obj->frame;

    for (i=0; i<obj->num_of_sprs; i++){
        _transfer_spr_attr_to_vram(0, 0, x, y, base_addr+(8*i), 1, &VRAM_sprattr+(8*(i+obj->spr_ind)));
    }
}

void play_obj_anim_frame(struct ObjectStruct *obj){
    int anim_addr = object_defs[obj->obj_type_ref].anim_addr;
    int num_of_sprs = object_defs[obj->obj_type_ref].num_of_sprs;
    int spr_addr = object_defs[obj->obj_type_ref].spr_addr;
    int pal_off = object_defs[obj->obj_type_ref].pal_off;

    int width = object_defs[obj->obj_type_ref].width;
    int height = object_defs[obj->obj_type_ref].height;

    int spr_ind = obj->spr_ind;

    char is_reverse = 0;
    int i=0;
    int x = ((((obj->x<<1) | (obj->frac_x>>7)) - width)-scroll_x);
    int y = ((((obj->y<<1) | (obj->frac_y>>7)) - height)-scroll_y);

    if(x < 512 & y < 512 & x > -64 & y > -64){
        int base_addr;

        if(!(obj->status & 0b00000010)){ is_reverse = 1; }
        base_addr = (anim_addr+3)+(is_reverse*8*num_of_sprs)+(2+16*num_of_sprs)*obj->frame;

        for (i=0; i<num_of_sprs; i++){
            _transfer_spr_attr_to_vram(pal_off, spr_addr, x, y, base_addr+(8*i), 1, &VRAM_sprattr+(8*(i+obj->spr_ind)));
        }
    }
}

void play_obj_anim(char num_of_frames, char *anim_ram_addr, struct ObjectStruct *obj){
    int anim_addr = object_defs[obj->obj_type_ref].anim_addr;
    char num_of_sprs = object_defs[obj->obj_type_ref].num_of_sprs;
    char *frame_count_addr;
    
    if(obj->anim_timer <= 0){
        obj->anim_index += 1;
        
        if(obj->anim_index >= num_of_frames){
            obj->anim_index = 0;
        }

        obj->frame = anim_ram_addr[obj->anim_index];

        frame_count_addr = (anim_addr+1)+(2+16*num_of_sprs) * (obj->frame);
        obj->anim_timer = *frame_count_addr;
    }
    
    obj->anim_timer -= 1;
}

void play_anim(char num_of_frames, char *anim_ram_addr, struct MegamanStruct *obj){
    char *frame_count_addr;

    if(obj->anim_timer <= 0){
        obj->anim_index += 1;
        
        if(obj->anim_index >= num_of_frames){
            obj->anim_index = 0;
        }
    
        obj->frame = anim_ram_addr[obj->anim_index];

        frame_count_addr = (obj->anim_addr+1)+(2+16*obj->num_of_sprs) * obj->frame;
        obj->anim_timer = *frame_count_addr;
    }
    
    obj->anim_timer -= 1;
    play_anim_frame(obj);
}

char get_pressed(int joystick, int button){
    if((joystick & button) != 0){
        return(1);
    }
    return(0);
}

void load_map_data(){
    char no_of_secs = *(int *)(map_info_addr+(lvl_num*room_data_size)+9);
    char i = 0;

    if(coll_data_addr != 0){ free(coll_data_addr); }
    if(spawn_data_addr != 0){ free(spawn_data_addr); }
    
    for(i=0; i<no_of_secs; i++){
        tile_map[8] = 0x30+(i);
        tile_map2[8] = 0x30+(i);

        coll_data_addr = malloc( *(int *)(map_info_addr+(lvl_num*room_data_size)+2));
        spawn_data_addr = malloc( *(int *)(map_info_addr+(lvl_num*room_data_size)+7));
        
        RAM_BANK_SEL = (i*2)+1;
        _load_file_into_ram(tile_map, 13, tile_map0_ram_addr);
        RAM_BANK_SEL = (i*2)+2;
        _load_file_into_ram(tile_map2, 13, tile_map1_ram_addr);
    }

    _load_file_into_ram(col_map, 10, coll_data_addr);
    _load_file_into_ram(spawn_map, 10, spawn_data_addr);
}