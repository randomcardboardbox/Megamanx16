#ifndef GLOBALS
#define GLOBALS

#include "object.h"
#include "megaman.h"

extern int scroll_x;
extern int scroll_y;
extern int map_info_addr;
extern int tile_map0_ram_addr;
extern int tile_map1_ram_addr;
extern int coll_data_addr;
extern char curr_room;
extern char lvl_num;
extern char room_data_size;
extern char last_room;

extern char tile_map[];
extern char tile_map2[];
extern char col_map[];

extern long map_l0_vram_addr;
extern long map_l1_vram_addr;
extern int tile_set_addr;

extern struct MegamanStruct megaman_obj;

extern struct ObjectReferenceStruct object_defs[10];
extern struct ObjectStruct objects[10];

#endif