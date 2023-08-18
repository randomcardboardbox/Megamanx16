#ifndef OBJECT
#define OBJECT

struct ObjectStruct
{
    char obj_type_ref;

    int x;
    char frac_x;
    int y;
    char frac_y;

    char x_vel;
    char x_frac_vel;
    char y_vel;
    char y_frac_vel;

    char health;
    char status;

    int anim_timer;
    char frame;
    char anim_index;
    char spr_ind;

    int timer1;
    int timer2;
    int timer3;

    char var1;
    char var2;
    char var3;
};

struct ObjectReferenceStruct
{
    int func_ptr;
    char func_bank;
    int anim_addr;
    char num_of_sprs;
    char pal_off;
};

#endif//OBJECT