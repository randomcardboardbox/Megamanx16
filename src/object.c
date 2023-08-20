#include <stdio.h>

#include "object.h"
#include "globals.h"

#include "met.h"

struct ObjectReferenceStruct object_defs[no_of_objs];
struct ObjectStruct objects[no_of_objs];

char obj_alloc_table[no_of_objs] = {0};

char alloc_obj(){
    int i = 0;
    for(i=0; i<no_of_objs; i++){
        if(obj_alloc_table[i] == 0){
            obj_alloc_table[i] = 1;
            return(i);
        }
    }
    return(no_of_objs-1);
}

void dealloc_obj(char obj_ind){
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

int _dis_to_megaman(struct ObjectStruct *obj){
    int x_dis = obj->x - megaman_obj.x;
    int y_dis = obj->y - megaman_obj.y;
    long dis_sq = (x_dis*x_dis)+(y_dis*y_dis);

    int dis = dis_sq >> 4;
    return(dis);
}