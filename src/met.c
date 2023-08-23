#include "met.h"
#include "globals.h"
#include "object.h"
#include "utils.h"

void bullet_update(char obj_ind){
    char anim[] = {0,1,2,3,4,5};
    _m_apply_velocity(&objects[obj_ind]);
    play_obj_anim(6, anim, obj_ind);
};

void bullet_draw(char obj_ind){
    // struct ObjectStruct *obj = &objects[obj_ind];
    // play_obj_anim_frame(obj);
}

void spawn_bullets(struct ObjectStruct *obj){
    char vel = 1;
    // char x_vel = 127;
    char y_vel = 127;
    char y_vel2 = 130;
    // char y_vel = 100;
    char obj_ind1 = alloc_obj();
    char obj_ind2 = alloc_obj();
    char obj_ind3 = alloc_obj();
    objects[obj_ind1].y = obj->y;
    if(obj->status & 0b00000010){
        objects[obj_ind1].x = obj->x+12; 
        objects[obj_ind1].x_vel = 1;
        objects[obj_ind1].x_frac_vel = 15;
    }
    else{
        objects[obj_ind1].x = obj->x-12; 
        objects[obj_ind1].x_vel = 254;
        objects[obj_ind1].x_frac_vel = 15;
    }
    objects[obj_ind1].obj_type_ref = 2;
    objects[obj_ind1].spr_ind = alloc_sprites(object_defs[2].num_of_sprs);
    objects[obj_ind1].spawn_id = 0;
    objects[obj_ind1].update_ptr = object_defs[2].update_ptr;
    objects[obj_ind1].draw_ptr = object_defs[2].draw_ptr;


    objects[obj_ind2].y = obj->y;
    objects[obj_ind3].y_vel = 0;
    objects[obj_ind2].y_frac_vel = y_vel;
    if(obj->status & 0b00000010){
        objects[obj_ind2].x = obj->x+12; 
        objects[obj_ind2].x_vel = 0;
        objects[obj_ind2].x_frac_vel = 175;
    }
    else{
        objects[obj_ind2].x = obj->x-12; 
        objects[obj_ind2].x_vel = -1;
        objects[obj_ind2].x_frac_vel = 175;
    }
    objects[obj_ind2].obj_type_ref = 2;
    objects[obj_ind2].spr_ind = alloc_sprites(object_defs[2].num_of_sprs);
    objects[obj_ind2].spawn_id = 0;
    objects[obj_ind2].update_ptr = object_defs[2].update_ptr;
    objects[obj_ind2].draw_ptr = object_defs[2].draw_ptr;



    objects[obj_ind3].y = obj->y;
    objects[obj_ind3].y_vel = 255;
    objects[obj_ind3].y_frac_vel = y_vel;
    if(obj->status & 0b00000010){
        objects[obj_ind3].x = obj->x+12; 
        objects[obj_ind3].x_vel = 0;
        objects[obj_ind3].x_frac_vel = 175;
    }
    else{
        objects[obj_ind3].x = obj->x-12; 
        objects[obj_ind3].x_vel = -1;
        objects[obj_ind3].x_frac_vel = 175;
    }
    objects[obj_ind3].obj_type_ref = 2;
    objects[obj_ind3].spr_ind = alloc_sprites(object_defs[2].num_of_sprs);
    objects[obj_ind3].spawn_id = 0;
    objects[obj_ind3].update_ptr = object_defs[2].update_ptr;
    objects[obj_ind3].draw_ptr = object_defs[2].draw_ptr;
}

void met_draw(char obj_ind){
    struct ObjectStruct *obj = &objects[obj_ind];
    // play_obj_anim_frame(obj);
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

    // play_obj_anim_frame(obj_ind);

    if(obj->var1 == 0){
        if(obj->timer1 == 0){
            dis_from_mm = _dis_to_megaman(obj);
            if(dis_from_mm < 200){
                obj->var1 = 1;
                obj->anim_index = 0;
                obj->anim_timer = 0;
            }
        }
        obj->frame = 0;
    }
    else if(obj->var1 == 1){
        play_obj_anim(9, m_open_anim, obj_ind);
        if(obj->anim_index == 8){
            obj->var1 = 2;
            obj->timer1 = 10;
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
        play_obj_anim(6, m_close_anim, obj_ind);
        if(obj->anim_index == 5){
            obj->var1 = 0;
            obj->timer1 = 45;
            obj->anim_index = 0;
            obj->anim_timer = 0;
        }
    }
}