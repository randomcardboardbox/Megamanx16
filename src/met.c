#include "met.h"
#include "globals.h"
#include "object.h"
#include "utils.h"
#include <stdio.h>

void bullet_update(char obj_ind){
    char anim[] = {0,1,2,3,4,5};
    _m_apply_velocity(&objects[obj_ind]);
    play_obj_anim(6, anim, &objects[obj_ind]);
};

void bullet_draw(char obj_ind){
    struct ObjectStruct *obj = &objects[obj_ind];
    play_obj_anim_frame(obj);
}

void spawn_bullets(struct ObjectStruct *obj){
    char vel = 1;
    // char x_vel = 127;
    char y_vel = 100;
    char y_vel2 = 156;
    // char y_vel = 100;
    char obj_ind1 = alloc_obj();
    char obj_ind2 = alloc_obj();
    char obj_ind3 = alloc_obj();
    objects[obj_ind1].y = obj->y+6;
    if(obj->status & 0b00000010){
        objects[obj_ind1].x = obj->x+12; 
        objects[obj_ind1].x_vel = vel;
        objects[obj_ind1].x_frac_vel = 15;
    }
    else{
        objects[obj_ind1].x = obj->x-12; 
        objects[obj_ind1].x_vel = -vel-1;
        objects[obj_ind1].x_frac_vel = 220;
    }
    objects[obj_ind1].obj_type_ref = 2;
    objects[obj_ind1].spr_ind = alloc_sprites(object_defs[2].num_of_sprs);



    objects[obj_ind2].y = obj->y+6;
    objects[obj_ind2].y_frac_vel = y_vel;
    if(obj->status & 0b00000010){
        objects[obj_ind2].x = obj->x+12; 
        objects[obj_ind2].x_vel = 1;
    }
    else{
        objects[obj_ind2].x = obj->x-12; 
        objects[obj_ind2].x_vel = -1;
    }
    objects[obj_ind2].obj_type_ref = 2;
    objects[obj_ind2].spr_ind = alloc_sprites(object_defs[2].num_of_sprs);



    objects[obj_ind3].y = obj->y+6;
    objects[obj_ind3].y_frac_vel = y_vel2;
    if(obj->status & 0b00000010){
        objects[obj_ind3].x = obj->x+12; 
        objects[obj_ind3].x_vel = 1;
    }
    else{
        objects[obj_ind3].x = obj->x-12; 
        objects[obj_ind3].x_vel = -1;
    }
    objects[obj_ind3].obj_type_ref = 2;
    objects[obj_ind3].spr_ind = alloc_sprites(object_defs[2].num_of_sprs);
}

void met_draw(char obj_ind){
    struct ObjectStruct *obj = &objects[obj_ind];
    play_obj_anim_frame(obj);
}

void met_update(char obj_ind){
    char m_open_anim[] = {0,1,2,3,4,5,6,7,8};
    char m_close_anim[] = {8,9,1,10,11,0};
    // var1:
    // 0-closed
    // 1-opening
    // 2-opened
    // 3-closing

    int dis_from_mm;
    struct ObjectStruct *obj = &objects[obj_ind];

    if(obj->var1 == 0){
        if(obj->timer1 == 0){
            dis_from_mm = _dis_to_megaman(obj);
            if(dis_from_mm < 400){
                obj->var1 = 1;
                 obj->anim_index = 0;
                 obj->anim_timer = 0;
            }
        }
        obj->frame = 0;
    }
    else if(obj->var1 == 1){
        play_obj_anim(9, m_open_anim, &objects[obj_ind]);
        if(obj->anim_index == 8){
            obj->var1 = 2;
            obj->timer1 = 20;
            obj->timer2 = 2;
            obj->anim_index = 0;
            obj->anim_timer = 0;
            obj->var2 = 0;
        }
    }
    else if(obj->var1 == 2){
        obj->frame = 8;
        if(obj->timer1 == 0){
            obj->var1 = 3;
            obj->anim_index = 0;
            obj->anim_timer = 0;
        }

        //spawn bullets
        if(obj->timer2 == 0 & obj->var2 == 0){
            spawn_bullets(obj);
            obj->var2 = 5;
        }
    }
    else if(obj->var1 == 3){
        play_obj_anim(6, m_close_anim, &objects[obj_ind]);
        if(obj->anim_index == 5){
            obj->var1 = 0;
            obj->timer1 = 50;
            obj->anim_index = 0;
            obj->anim_timer = 0;
        }
    }
}