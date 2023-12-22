#include "object.h"
#include "globals.h"
#include "utils.h"

#include "met.h"

struct ObjectReferenceStruct object_defs[no_of_objtypes];
struct ObjectStruct objects[no_of_objs];
struct ObjectStruct bullet_objects[3];

char enemy_objects[NO_OF_ENEMIES];

char obj_alloc_table[no_of_objs] = {0};
char spawned_objs_table[50] = {0};

char alloc_obj(){
    int i = 0;
    for(i=0; i<no_of_objs; i++){
        if(obj_alloc_table[i] == 0){
            obj_alloc_table[i] = 1;
            objects[i].spr_ind = NULL_SPRITE;
            return(i);
        }
    }

    dealloc_obj(no_of_objs-1);
    objects[no_of_objs-1].spr_ind = NULL_SPRITE;
    return(no_of_objs-1);
}

void clear_objs(){
    int i = 0;
    for(i=0; i<no_of_objs; i++){
        if(obj_alloc_table[i]){
            dealloc_obj(i);
        }
    }
}


void dealloc_obj(char obj_ind){
    struct ObjectStruct object = {0,0,0,0, 0,0,0,0, 0,0, 0,0, 0,0,0,0, 0,0,0, 0,0,0};

    if(objects[obj_ind].spr_ind != NULL_SPRITE){
        dealloc_sprites(objects[obj_ind].spr_ind);
    }
    spawned_objs_table[objects[obj_ind].spawn_id] = 0;

    // if(object_defs[objects[obj_ind].obj_type_ref].interaction_type == 1){
    //     char i = 0;
    //     for (i=0; i<NO_OF_ENEMIES; i++){
    //         if(enemy_objects[i] == obj_ind){
    //             enemy_objects[i] = 127;
    //             break;
    //         }
    //     }
    // }

    objects[obj_ind].spr_ind = NULL_SPRITE;
    objects[obj_ind] = object;
    objects[obj_ind].obj_type_ref = 0;
    obj_alloc_table[obj_ind] = 0;
}

char draw_even_objs = 1;
void draw_objects(void){
    char i = 0;

    for(i=draw_even_objs; i<no_of_objs; i+=2){
        if(obj_alloc_table[i] == 0){
            continue; }

        // object_defs[objects[i].obj_type_ref].draw_ptr(i);
        play_obj_anim_frame(i);
    }

    if(draw_even_objs){
        draw_even_objs = 0;
    }
    else{
        draw_even_objs = 1;
    }
}

void empty_obj_func(char obj_ind){

}

void update_objects(void){
    char i = 0;

    for(i=0; i<no_of_objs; i++){
        int obj_x_pos = (objects[i].x << 1) | (objects[i].frac_x >> 7);
        
        if(obj_alloc_table[i] == 0){
            continue; }

        if(scroll_x > obj_x_pos && scroll_x - obj_x_pos > 520){
            dealloc_obj(i);
            continue;
        }
        else if(scroll_x < obj_x_pos && obj_x_pos - scroll_x > 520){
            dealloc_obj(i);
            continue;
        }
        else if(objects[i].y>512 || objects[i].y<0){
            dealloc_obj(i);
            continue;
        }

        object_defs[objects[i].obj_type_ref].update_ptr(i);
        
        if(objects[i].timer1  > 0){
        objects[i].timer1 -= 1;}
        
        if(objects[i].timer2  > 0){
        objects[i].timer2 -= 1;}

        if(objects[i].timer3  > 0){
        objects[i].timer3 -= 1;}
    }
}

void spawn_check(int chunk){
    int i = 0;

    for(i=0; i<objs_per_chunk; i++){
        char obj_ref = *(char *)(spawn_data_addr+(objs_per_chunk*chunk*6)+(i*6));
        char spawn_id = *(char *)(spawn_data_addr+(objs_per_chunk*chunk*6)+(i*6)+5);
        RAM_BANK_SEL = lvl_data_bank;
        
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
            objects[obj_ind].draw_ptr = object_defs[obj_ref].draw_ptr;
            objects[obj_ind].update_ptr = object_defs[obj_ref].update_ptr;
            spawned_objs_table[spawn_id] = 1;

            // if(object_defs[obj_ref].interaction_type == 1){
            //     char i = 0;
            //     for (i=0; i<NO_OF_ENEMIES; i++){
            //         if(enemy_objects[i] == 127){
            //             enemy_objects[i] = obj_ind;
            //             break;
            //         }
            //     }
            // }
        }
    }
}

int _dis_to_megaman(struct ObjectStruct *obj){
    int x_dis = (obj->x - megaman_obj.x) >> 1;
    int y_dis = (obj->y - megaman_obj.y) >> 1;
    long dis_sq = (x_dis*x_dis)+(y_dis*y_dis);

    int dis = dis_sq >> 3;
    return(dis);
}

char _megaman_dir(struct ObjectStruct *obj){
    if(obj->x < megaman_obj.x){
        return(1);
    }
    return(0);
}

// TODO: Untested collision between two objects
char _collision_with_object(char obj1, char obj2){
    char width = object_defs[objects[obj1].obj_type_ref].width + object_defs[objects[obj2].obj_type_ref].width;
    char height = object_defs[objects[obj1].obj_type_ref].height + object_defs[objects[obj2].obj_type_ref].height;

    if(objects[obj1].x > objects[obj2].x && objects[obj1].x-objects[obj2].x > width){
        return(0);
    }
    if(objects[obj1].x < objects[obj2].x && objects[obj2].x-objects[obj1].x > width){
        return(0);
    }
    
    if(objects[obj1].y > objects[obj2].y && objects[obj1].y-objects[obj2].y > height){
        return(0);
    }
    if(objects[obj1].y < objects[obj1].y && objects[obj2].y-objects[obj1].y > height){
        return(0);
    }

    return(1);
}

char _collision_with_bullet(char obj_ind){
    char i = 0;
    char width = object_defs[objects[obj_ind].obj_type_ref].width+2;
    char height = object_defs[objects[obj_ind].obj_type_ref].height+2;


    for (i=0; i<3; i++){
        if(bullet_objects[i].obj_type_ref == 0){
            continue;
        }

        if(objects[obj_ind].x > bullet_objects[i].x && objects[obj_ind].x-bullet_objects[i].x > width){
            continue;
        }
        if(objects[obj_ind].x < bullet_objects[i].x && bullet_objects[i].x-objects[obj_ind].x > width){
            continue;
        }
        
        if(objects[obj_ind].y > bullet_objects[i].y && objects[obj_ind].y-bullet_objects[i].y > height){
            continue;
        }
        if(objects[obj_ind].y < bullet_objects[i].y && bullet_objects[i].y-objects[obj_ind].y > height){
            continue;
        }
        return(i+1);
    }
    return(0);
}

char _collided_with_megaman(struct ObjectStruct *obj){
    // TODO: fix collision
    int x = megaman_obj.x;
    int y = megaman_obj.y;

    char width = object_defs[obj->obj_type_ref].width + 8;
    char height = object_defs[obj->obj_type_ref].height + 8;

    char collided = 0;
    if(megaman_obj.status & 0b00000010){
        x += 2;}
    else{x -= 2;}

    if(x > obj->x && x-obj->x > width){
        return(0);
    }
    if(x < obj->x && obj->x-x > width){
        return(0);
    }
    
    if(y > obj->y && y-obj->y > height){
        return(0);
    }
    if(y < obj->y && obj->y-y > height){
        return(0);
    }

    return(1);

}