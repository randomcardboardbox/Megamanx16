#ifndef OBJECT
#define OBJECT

struct ObjectStruct
{
    int x;
    char frac_x;
    int y;
    char frac_y;

    char x_vel;
    char x_frac_vel;
    char y_vel;
    char y_frac_vel;

    char obj_type_ref;

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
    void (*update_ptr)(char);
    void (*draw_ptr)(char);
    char func_bank;
    int anim_addr;
    int spr_addr;
    char num_of_sprs;
    char pal_off;
    char width;
    char height;
};

void update_objs(void);
void draw_objs(void);
char alloc_obj();
void dealloc_obj(char obj_ind);

int _dis_to_megaman(struct ObjectStruct *obj);
void empty_obj_func(char obj_ind);

#endif//OBJECT