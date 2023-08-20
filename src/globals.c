int scroll_x;
int scroll_y;

int tile_map0_ram_addr = 0xA000;
int tile_map1_ram_addr = 0xA000;
int coll_data_addr = 0x0000;
int spawn_data_addr = 0x0000;

char curr_room=0;

char lvl_num = 0;
char room_data_size = 10;
int map_info_addr = 0;
char last_room;

char tile_map[] =   "gutr00l00.stg";
char tile_map2[] =  "gutr00l10.stg";
char col_map[] =    "gutc00.stg";
char spawn_map[] =  "guts00.stg";

long map_l0_vram_addr   = 0x03800;
long map_l1_vram_addr   = 0x04800;
int tile_set_addr      = 0x058; // mulitply by 8 to get the actual addr

char objs_per_chunk = 3;