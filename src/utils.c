#include <stdlib.h>

#include "globals.h"
#include "utils.h"

char spr_alloc_table[128] = {0};

char alloc_sprites(char size){
    int i=0;
    int j=0;
    int init_ind=0;

    for (i=0; i<(128-size); i++){
        if(spr_alloc_table[i] != 0){
            init_ind = i;
        }
        if((i-init_ind)==(size-1)){
            spr_alloc_table[init_ind] = size;
            for (j=1; j<size; j++){
                spr_alloc_table[init_ind+j] = 1;
            }
            return(init_ind);
        }
    }
    return(127-size);
}

void dealloc_sprites(char index){
    int i=0;
    int size=spr_alloc_table[index+i];

    for (i=0; i<size; i++){
        spr_alloc_table[index+i] = 0;
        _clear_sprite_attribute(index+i);
    }
}


void play_anim_frame(struct MegamanStruct *obj){
    char is_reverse = 0;
    int i=0;
    int x = ((((obj->x<<1) | (obj->frac_x>>7)) - 32)-scroll_x)&0b0000000111111111;
    int y = ((((obj->y<<1) | (obj->frac_y>>7)) - 32)-scroll_y)&0b0000000111111111;

    int base_addr;

    if(!(obj->status & 0b00000010)){ is_reverse = 1; }
    base_addr = (obj->anim_addr+3)+(is_reverse*8*obj->num_of_sprs)+(2+16*obj->num_of_sprs)*obj->frame;

    for (i=0; i<obj->num_of_sprs; i++){
        _transfer_spr_attr_to_vram(x, y, base_addr+(8*i), 1, &VRAM_sprattr+(8*(i+obj->spr_ind)));
    }
}

void play_anim(char num_of_frames, char *anim_ram_addr, struct MegamanStruct *obj){
    char *frame_count_addr = (obj->anim_addr+1)+(2+16*obj->num_of_sprs) * anim_ram_addr[obj->anim_index];
    if(obj->anim_timer <= 1){
        obj->anim_index += 1;
        obj->anim_timer = *frame_count_addr;
        
        if(obj->anim_index >= num_of_frames){
            obj->anim_index = 0;
        }

        obj->frame = anim_ram_addr[obj->anim_index];
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
    if(coll_data_addr != 0){
        free(coll_data_addr);
    }

    coll_data_addr = malloc( *(int *)(map_info_addr+(lvl_num*room_data_size)+2));
    RAM_BANK_SEL = 1;
    _load_file_into_ram(tile_map, 12, tile_map0_ram_addr);
    RAM_BANK_SEL = 2;
    _load_file_into_ram(tile_map2, 12, tile_map1_ram_addr);
    _load_file_into_ram(col_map, 10, coll_data_addr);
}