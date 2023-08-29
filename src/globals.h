#ifndef GLOBALS
#define GLOBALS

#include "object.h"
#include "megaman.h"

#define no_of_objs 30
#define no_of_objtypes 5
#define lvl_data_bank 1
#define tile_set_ram_bank 2

extern int scroll_x;
extern int scroll_y;
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

extern char objs_per_chunk;

extern struct MegamanStruct megaman_obj;

extern struct ObjectReferenceStruct object_defs[no_of_objtypes];
extern struct ObjectStruct objects[no_of_objs];

#endif