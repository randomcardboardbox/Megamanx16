#include <stdio.h>

#include "object.h"
#include "globals.h"
#include "utils.h"

#include "met.h"

struct ObjectReferenceStruct object_defs[no_of_objs];
struct ObjectStruct objects[no_of_objs];

char obj_alloc_table[no_of_objs] = {0};
char spawned_objs_table[50] = {0};

char alloc_obj(){
    int i = 0;
    for(i=0; i<no_of_objs; i++){
        if(obj_alloc_table[i] == 0){
            obj_alloc_table[i] = 1;
            return(i);
        }
    }

    dealloc_obj(no_of_objs-1);
    return(no_of_objs-1);
}

void dealloc_obj(char obj_ind){
    struct ObjectStruct object = {0,0,0,0, 0,0,0,0, 0,0, 0,0, 0,0,0,0, 0,0,0, 0,0,0};

    dealloc_sprites(objects[obj_ind].spr_ind);
    spawned_objs_table[objects[obj_ind].spawn_id] = 0;

    objects[obj_ind] = object;
    obj_alloc_table[obj_ind] = 0;
}

void update_objs(void){
    char i = 0;
    struct ObjectStruct object;

    for(i=0; i<no_of_objs; i++){
        if(obj_alloc_table[i] == 0){
            continue; }

        object_defs[objects[i].obj_type_ref].update_ptr(i);

        if(objects[i].timer1 != 0){ objects[i].timer1 -= 1;}
        if(objects[i].timer2 != 0){ objects[i].timer2 -= 1;}
        if(objects[i].timer3 != 0){ objects[i].timer3 -= 1;}

        check_despawn(i);
    }
}

void draw_objs(void){
    char i = 0;
    struct ObjectStruct object;

    for(i=0; i<no_of_objs; i++){
        if(obj_alloc_table[i] == 0){
            continue; }

        object_defs[objects[i].obj_type_ref].draw_ptr(i);
    }
}

void empty_obj_func(char obj_ind){

}

void spawn_check(int chunk){
    int i = 0;

    for(i=0; i<objs_per_chunk; i++){
        char obj_ref = *(char *)(spawn_data_addr+(objs_per_chunk*chunk*6)+(i*6));
        char spawn_id = *(char *)(spawn_data_addr+(objs_per_chunk*chunk*6)+(i*6)+5);
        
        if(obj_ref != 0 && spawned_objs_table[spawn_id] == 0){
            int x = *(int *)(spawn_data_addr+(objs_per_chunk*chunk*6)+(i*6)+1);
            int y = *(int *)(spawn_data_addr+(objs_per_chunk*chunk*6)+(i*6)+3);
            int obj_ind;
            
            obj_ind = alloc_obj();
            objects[obj_ind].x = x;
            objects[obj_ind].y = y;
            objects[obj_ind].obj_type_ref = obj_ref;
            objects[obj_ind].spr_ind = alloc_sprites(object_defs[obj_ref].num_of_sprs);
            objects[obj_ind].spawn_id = spawn_id;
            spawned_objs_table[spawn_id] = 1;
        }
    }
}

void check_despawn(char obj_ind){
    struct ObjectStruct *obj = &objects[obj_ind];
    int dif_x = (obj->x - scroll_x/2) >> 1;
    int dif_x_sq = dif_x*dif_x;

    int dif_y = (obj->y - scroll_y/2) >> 1;
    int dif_y_sq = dif_y*dif_y;

    if(dif_x_sq > 17000 || dif_y_sq > 17000){
        dealloc_obj(obj_ind);
    }    
}


int _dis_to_megaman(struct ObjectStruct *obj){
    int x_dis = (obj->x - megaman_obj.x) >> 1;
    int y_dis = (obj->y - megaman_obj.y) >> 1;
    long dis_sq = (x_dis*x_dis)+(y_dis*y_dis);

    int dis = dis_sq >> 3;
    return(dis);
}