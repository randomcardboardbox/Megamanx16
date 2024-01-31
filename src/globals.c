#include <cbm.h>

int scroll_x;
int scroll_y;
int scroll_x_bg;
int scroll_y_bg;

int tile_map0_ram_addr = 0xA000;
int tile_map1_ram_addr = 0xA000;
int coll_data_addr = 0x0000;
int spawn_data_addr = 0x0000;

char curr_room=0;

char lvl_num = 0;
char room_data_size = 12;
int map_info_addr = 0;
char last_room;

char tile_map[] =   "gutr00l00.stg";
char tile_map2[] =  "gutr00l10.stg";
char col_map[] =    "gutc00.stg";
char spawn_map[] =  "guts00.stg";

long map_l0_vram_addr   = 0x06800;
long map_l1_vram_addr   = 0x07800;
int tile_set_addr      = 0x090; // mulitply by 8 to get the actual addr
int tile_set_addr2      = 0x160;

char objs_per_chunk = 3;
char bg_parallax_x;
char bg_parallax_y;

char object_code_block = 2;

char initial_health = 29;
char flush_timer = 0;
char started_flush = 0;
char seg_flush_time = 0;

void default_level_update(void){};
void default_level_transition_start(void){};
void default_level_transition_end(void){};
void default_level_transition_update(void){};

level_update = &default_level_update;
level_transition_start = &default_level_transition_start;
level_transition_end = &default_level_transition_end;
level_transition_update = &default_level_transition_update;