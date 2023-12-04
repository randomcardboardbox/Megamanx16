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
    char spawn_id;

    char health;
    char status; // -/-/-/-/ is_invincible / direction / is_grounded

    int anim_timer;
    char frame;
    char anim_index;
    char spr_ind;

    unsigned int timer1;
    unsigned int timer2;
    unsigned int timer3;

    char var1;
    char var2;
    char var3;

    void (*update_ptr)(char);
    void (*draw_ptr)(char);
};

struct ObjectReferenceStruct
{
    void (*update_ptr)(char);
    void (*draw_ptr)(char);
    char upadte_func_bank;
    char draw_func_bank;
    int anim_addr;
    int spr_addr;
    char num_of_sprs;
    char pal_off;
    char spr_width;
    char spr_height;
    char width;
    char height;
};

void draw_objs(void);
char alloc_obj();
void dealloc_obj(char obj_ind);
void clear_objs();

char _megaman_dir(struct ObjectStruct *obj);
int _dis_to_megaman(struct ObjectStruct *obj);
char _collided_with_megaman(struct ObjectStruct *obj);
void empty_obj_func(char obj_ind);
void spawn_check(int chunk);
void check_despawn(char obj_ind);

extern void _update_objects(int obj_arr_addr, int scroll_x, void (*dealloc_func)(char));
extern void _draw_objects(void (*draw_func)(char), int obj_arr_addr);


#endif//OBJECT