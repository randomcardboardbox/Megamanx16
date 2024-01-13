#ifndef GLOBALS
#define GLOBALS

#include "object.h"
#include "megaman.h"

#define no_of_objs 20
#define no_of_objtypes 8
#define lvl_data_bank 16
#define tile_set_ram_bank 17

extern int scroll_x;
extern int scroll_y;
extern int scroll_x_bg;
extern int scroll_y_bg;
extern int map_info_addr;
extern int tile_map0_ram_addr;
extern int tile_map1_ram_addr;
extern int coll_data_addr;
extern int spawn_data_addr;
extern char curr_room;
extern char lvl_num;
extern char room_data_size;
extern char last_room;

extern char tile_map[];
extern char tile_map2[];
extern char col_map[];
extern char spawn_map[];

extern long map_l0_vram_addr;
extern long map_l1_vram_addr;
extern int tile_set_addr;
extern int tile_set_addr2;

extern char objs_per_chunk;
extern char bg_parallax_x;
extern char bg_parallax_y;

extern struct MegamanStruct megaman_obj;

extern struct ObjectReferenceStruct object_defs[no_of_objtypes];
extern struct ObjectStruct objects[no_of_objs];
extern struct ObjectStruct bullet_objects[3];

extern char initial_health;
extern char flush_timer;
extern char started_flush;
extern char seg_flush_time;

extern char object_code_block;

extern void (*level_update)(void);
extern void (*level_transition_start)(char level_num);
extern void (*level_transition_end)(char level_num);
extern void (*level_transition_update)(char level_num, int frame_index);

#endif