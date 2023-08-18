import json
import math

#TODO: Export layer1 edit layer along with layer 1
#TODO: Read in the aseprite file to allow for maore accurate tile saving

asset_path = r"assets/tilemaps/guts_man_stage_tilemap.ldtk"
ts_path = r"assets/tilemaps/gut.json"
filename = "x16_file_sys/GUT"
extension = ".STG"
init_pal_offset = 2
dir_arr = [None, "Right", "Left", "Up", "Down"]

def find_enum(enums, tag):
    for enum in enums:
        if(enum["enumValueId"] == tag):
            return(enum)
    return(None)

class map_tile:
    def __init__(self, tile_ids, flip_bits, pal_offset=0, coll_data=0):
        global init_pal_offset
        self.tile_ids = tile_ids
        self.flip_bits = flip_bits
        self.pal_offset = pal_offset + init_pal_offset
        self.coll_data = coll_data

def write_bytes(tile_id, flip_bits, pal_offset, file):
    byte0 = (tile_id%256).to_bytes(1, "little")
    byte1 = (((tile_id >> 8) & 3) | (flip_bits << 2) | (pal_offset << 4)).to_bytes(1, "little")
    file.write(byte0)
    file.write(byte1)

def save_tilemap_to_file(tile_arr, tile_map_off, lvl_width, lvl_height, file):
    if(lvl_width < lvl_height):
        end_tile = min(64, len(tile_arr)-tile_map_off)
        for j in range(tile_map_off, tile_map_off+end_tile):
            for i in range(len(tile_arr[0])):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[0], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[2], tile.flip_bits, tile.pal_offset, file)

            for i in range(len(tile_arr[0])):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[1], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[3], tile.flip_bits, tile.pal_offset, file)

    else:
        end_tile = min(64, len(tile_arr[0])-tile_map_off)
        for i in range(tile_map_off, tile_map_off+end_tile):
            for j in range(len(tile_arr)):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[0], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[2], tile.flip_bits, tile.pal_offset, file)
            
            for j in range(len(tile_arr)):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[1], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[3], tile.flip_bits, tile.pal_offset, file)

def save_collision_to_file(tile_arr, lvl_width, lvl_height, file):
    if(lvl_width < lvl_height):
        for j in range(len(tile_arr)):
            for i in range(len(tile_arr[0])):
                tile = tile_arr[j][i]
                byte = tile.coll_data.to_bytes(1, "little")
                file.write(byte)
    
    else:
        for i in range(len(tile_arr[0])):
            for j in range(len(tile_arr)):
                tile = tile_arr[j][i]
                byte = tile.coll_data.to_bytes(1, "little")
                file.write(byte)
    
def append_tile_to_arr(grid_tile, arr, coll_arr):
    tile_ids = metatiles[grid_tile["t"]]
    
    pal_off = 0
    if(grid_tile["t"] in pall_off0_tiles): pal_off = 0
    if(grid_tile["t"] in pall_off1_tiles): pal_off = 1


    if(grid_tile["t"] in coll_arr): coll_data = 255
    else: coll_data = 0

    new_tile = map_tile(tile_ids, grid_tile["f"], pal_off, coll_data)
    pos = [grid_tile["px"][0], grid_tile["px"][1]]
    pos = [int(pos[0]/tile_size), int(pos[1]/tile_size)]
    arr[pos[1]][pos[0]] = new_tile
        

def parse_level(tile_sec, grid_tiles, lvl_width, lvl_height, tileset, file, overwrite_tile_map=None):
    tile_arr = [[map_tile([0,0,0,0],0) for i in range(lvl_width)] for j in range(lvl_height)]
    coll_arr = find_enum(tileset["enumTags"], "Wall")["tileIds"]
    
    for grid_tile in grid_tiles:
        append_tile_to_arr(grid_tile, tile_arr, coll_arr)

    if(overwrite_tile_map is not None):
        for overwrite_tile in overwrite_tile_map:
            append_tile_to_arr(overwrite_tile, tile_arr, coll_arr)

    save_tilemap_to_file(tile_arr, tile_sec*64, lvl_width, lvl_height, file)
    return(tile_arr)
    

with open(asset_path, "r") as f:
    data = json.load(f)

levels = data["levels"]

tileset = data["defs"]["tilesets"][0]
tile_size = int(tileset["tileGridSize"])

metatiles = []
with open(ts_path, "r") as f:
    metatiles = json.load(f)

pall_off0_tiles = find_enum(tileset["enumTags"], "PALOFF0")["tileIds"]
pall_off1_tiles = find_enum(tileset["enumTags"], "PALOFF1")["tileIds"]

# create information file for level
full_filename = filename + "I" + extension
with open(full_filename, "wb") as info_file:
    info_file.write((0).to_bytes(2, "little"))
        

    for i, level in enumerate(levels):
        level = data["levels"][i]
        lvl_width = int(level["pxWid"] / tile_size)
        lvl_height = int(level["pxHei"] / tile_size)
        
        lvl_byte_size = math.ceil((level["pxWid"]*level["pxHei"]) / (tile_size*tile_size/4)) * 2
        col_byte_size = math.ceil(lvl_width * lvl_height) 
        
        if(lvl_width < lvl_height):
            num_of_rooms = int(lvl_height / 16)
        else:
            num_of_rooms = int(lvl_width / 16)

        info_file.write(lvl_byte_size.to_bytes(2,"little"))
        info_file.write(col_byte_size.to_bytes(2,"little"))
        info_file.write(num_of_rooms.to_bytes(1,"little"))

        enter_dir = dir_arr.index(level["fieldInstances"][0]["__value"])
        exit_dir = dir_arr.index(level["fieldInstances"][1]["__value"])
        
        info_file.write(enter_dir.to_bytes(1,"little"))
        info_file.write(exit_dir.to_bytes(1,"little"))

        if(lvl_width > lvl_height): no_of_secs = math.ceil(lvl_width/64)
        else: no_of_secs = math.ceil(lvl_height/64)

        info_file.write(no_of_secs.to_bytes(1,"little"))

        layer0 = level["layerInstances"][2]
        layer1 = level["layerInstances"][1]
        layer1_edit = level["layerInstances"][0]

        for j in range(no_of_secs):
            #create file for layer0 and layer1
            full_filename = filename + "R" + str(i).zfill(2) + "L" + "0" + str(j) + extension
            with open(full_filename, "wb") as f:
                f.write((0).to_bytes(2, "little"))
                parse_level(j, layer0["gridTiles"], lvl_width, lvl_height, tileset, f)

            full_filename = filename + "R" + str(i).zfill(2) + "L" + "1" + str(j) + extension
            with open(full_filename, "wb") as f:
                f.write((0).to_bytes(2, "little"))
                tile_arr = parse_level(j, layer1["autoLayerTiles"], lvl_width, lvl_height, tileset, f, layer1_edit["gridTiles"])

        full_filename = filename + "C" + str(i).zfill(2) + extension
        with open(full_filename, "wb") as f:
            f.write((0).to_bytes(2, "little"))
            save_collision_to_file(tile_arr, lvl_width, lvl_height, f)
