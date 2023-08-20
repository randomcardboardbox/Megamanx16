#ifndef MEGAMAN
#define MEGAMAN

struct MegamanStruct {
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
    // -,-,-,-,-,is_running,face-dir,is_grounded

    int anim_timer;
    char frame;
    char anim_index;
    int anim_addr;
    char num_of_sprs;
    char spr_ind;  
};

extern void _m_apply_velocity(int obj_addr);

void move_megaman_obj(void);
void update_megaman(void);
void check_collision(void);
void animate_megaman(void);
void load_megaman_spr_data(void);

#endif //MEGAMAN