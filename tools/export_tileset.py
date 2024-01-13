import json
import math

#TODO: Export layer1 edit layer along with layer 1
#TODO: Read in the aseprite file to allow for maore accurate tile saving

asset_path = r"assets/tilemaps/guts_man_stage_tilemap.ldtk"
ts_paths = {3  :r"assets/tilemaps/gut.json",
            143:r"assets/tilemaps/gut2.json",
            136:None,
            145:r"assets/tilemaps/gutu.json"
            }

filename = "x16_file_sys/GUT"
extension = ".STG"
init_pal_offset = 4
dir_arr = [None, "Right", "Left", "Up", "Down"]
OBJS_PER_CHUNK = 3

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
    byte1 = (((tile_id >> 8) & 3) | ((flip_bits % 4) << 2) | (pal_offset << 4)).to_bytes(1, "little")
    file.write(byte0)
    file.write(byte1)

def save_tilemap_to_file(tile_arr, tile_map_off, lvl_width, lvl_height, file):
    ORDER_OF_SAVE = [
            [0,2,1,3],
            [1,3,0,2],
            [2,0,3,1],
            [3,1,2,0],
    ]
    if(lvl_width < lvl_height):
        end_tile = min(64, len(tile_arr)-tile_map_off)
        for j in range(tile_map_off, tile_map_off+end_tile):
            for i in range(len(tile_arr[0])):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][0]], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][1]], tile.flip_bits, tile.pal_offset, file)

            for i in range(len(tile_arr[0])):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][2]], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][3]], tile.flip_bits, tile.pal_offset, file)

    else:
        end_tile = min(64, len(tile_arr[0])-tile_map_off)
        for i in range(tile_map_off, tile_map_off+end_tile):
            for j in range(len(tile_arr)):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][0]], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][1]], tile.flip_bits, tile.pal_offset, file)
                
            for j in range(len(tile_arr)):
                tile = tile_arr[j][i]
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][2]], tile.flip_bits, tile.pal_offset, file)
                write_bytes(tile.tile_ids[ORDER_OF_SAVE[tile.flip_bits][3]], tile.flip_bits, tile.pal_offset, file)

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
    
def append_tile_to_arr(grid_tile, arr, coll_arr, tile_def, pall_offsets):
    if(grid_tile["t"] < len(tile_def)):
        tile_ids = tile_def[grid_tile["t"]]
    else:
        tile_ids = [0,0,0,0]
    
    pal_off = 0
    for i in range(len(pall_offsets)):
        if(grid_tile["t"] in pall_offsets[i]):
            pal_off = i
            break


    if(grid_tile["t"] in coll_arr): coll_data = 255
    else: coll_data = 0

    new_tile = map_tile(tile_ids, grid_tile["f"], pal_off, coll_data)
    pos = [grid_tile["px"][0], grid_tile["px"][1]]
    pos = [int(pos[0]/tile_size), int(pos[1]/tile_size)]
    arr[pos[1]][pos[0]] = new_tile
        

def parse_level(tile_sec, tileset_uid, grid_tiles, lvl_width, lvl_height, file, overwrite_tile_map=None, overwrite_tileset_uid=None):
    tset = tilesets[tileset_uid]
    tile_def = metatiles_arr[tileset_uid]

    tset_overwrite = None
    tile_def_overwrite = None
    coll_arr_overwrite = None

    if(overwrite_tileset_uid is not None):
        tset_overwrite = tilesets[overwrite_tileset_uid]
        tile_def_overwrite = metatiles_arr[overwrite_tileset_uid]
        coll_arr_overwrite = find_enum(tset_overwrite["enumTags"], "Wall")["tileIds"]
    
    tile_arr = [[map_tile([0,0,0,0],0) for i in range(lvl_width)] for j in range(lvl_height)]
    coll_arr = find_enum(tset["enumTags"], "Wall")["tileIds"]
    
    for grid_tile in grid_tiles:
        if(overwrite_tile_map is None):
            append_tile_to_arr(grid_tile, tile_arr, coll_arr, tile_def, pall_offsets_arr[tileset_uid])
        else:
            append_tile_to_arr(grid_tile, tile_arr, coll_arr, tile_def, pall_offsets_arr[tileset_uid])

    if(overwrite_tile_map is not None):
        for overwrite_tile in overwrite_tile_map:
            append_tile_to_arr(overwrite_tile, tile_arr, coll_arr_overwrite, tile_def_overwrite, pall_offsets_arr[overwrite_tileset_uid])

    save_tilemap_to_file(tile_arr, tile_sec*64, lvl_width, lvl_height, file)
    return(tile_arr)


class entity:
    def __init__(self, obj_ref, x, y, is_right):
        self.obj_ref = obj_ref
        self.x = x
        self.y = y
        self.is_right = is_right

def find_entity_field(entity_fields, key):
    for field in entity_fields:
        if(field["__identifier"] == key):
            return(field)
    else:
        return(None)
    
def find_level(levels, lvl_str):
    for level in levels:
        if(level["__identifier"] == lvl_str):
            return(level)
        
    return(None)

def find_tileset(tilesets, tile_str):
    for tileset in tilesets:
        if(tileset["identifier"] == tile_str):
            return(tileset)
        
    return(None)

def parse_entities(entities, lvl_width, lvl_height, file):
    obj_arr = []
    obj_ind_arr = []

    if(lvl_width >= lvl_height):
        arr_size = lvl_width+1
    else: 
        arr_size = lvl_height+1

    for i in range(arr_size):
        obj_arr.append([])
        obj_ind_arr.append(0)
        for j in range(OBJS_PER_CHUNK):
            obj_arr[i].append(entity(0,0,0,True))

    for entity_inst in entities["entityInstances"]:
        if(lvl_width >= lvl_height):
            arr_ind = int(entity_inst["__grid"][0]/2)
        else:
            arr_ind = int(entity_inst["__grid"][1]/2)
        obj_arr[arr_ind][obj_ind_arr[arr_ind]].obj_ref = find_entity_field(entity_inst["fieldInstances"], "objRef")["__value"]
        obj_arr[arr_ind][obj_ind_arr[arr_ind]].is_right = find_entity_field(entity_inst["fieldInstances"], "isRight")["__value"]
        obj_arr[arr_ind][obj_ind_arr[arr_ind]].x = int(entity_inst["px"][0]/2)
        obj_arr[arr_ind][obj_ind_arr[arr_ind]].y = int(entity_inst["px"][1]/2)

        obj_ind_arr[arr_ind] += 1

    spawn_id = 1
    for chunk in obj_arr:
        for i in range(OBJS_PER_CHUNK):
            entity_inst = chunk[i]
            if(entity_inst.obj_ref != 0):
                file.write(entity_inst.obj_ref.to_bytes(1, "little"))
                file.write(entity_inst.x.to_bytes(2, "little"))
                file.write(entity_inst.y.to_bytes(2, "little"))
                file.write(spawn_id.to_bytes(1, "little"))
                spawn_id += 1
            else:
                file.write((0).to_bytes(6, "little"))
    

def get_palette_offsets(uid):
    pall_offsets = []
    MAX_PAL_OFFSETS = 16
    for i in range(MAX_PAL_OFFSETS):
        pal_str = "PALOFF" + str(i)
        pal_arr = find_enum(tilesets[uid]["enumTags"], pal_str)

        if(pal_arr is None):
            return(pall_offsets)

        pall_offsets.append(pal_arr["tileIds"])

    return(pall_offsets)

with open(asset_path, "r") as f:
    data = json.load(f)

levels = data["levels"]
tilesets = { int(i["uid"]):i for i in data["defs"]["tilesets"] }
tile_size = int(list(tilesets.values())[0]["tileGridSize"])

metatiles_arr = {}
pall_offsets_arr = {}
for k, v in ts_paths.items():
    if(ts_paths[k] is not None):
        with open(v, "r") as f:
            metatiles_arr[k] = json.load(f)

        pall_offsets_arr[k] = get_palette_offsets(k)
    else:
        metatiles_arr[k] = None
        pall_offsets_arr[k] = None


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
            spawn_byte_size = lvl_width * OBJS_PER_CHUNK * 5
        else:
            num_of_rooms = int(lvl_width / 16)
            spawn_byte_size = lvl_width * OBJS_PER_CHUNK * 5

        info_file.write(lvl_byte_size.to_bytes(2,"little"))
        info_file.write(col_byte_size.to_bytes(2,"little"))
        info_file.write((0).to_bytes(2,"little"))
        info_file.write(num_of_rooms.to_bytes(1,"little"))

        enter_dir = dir_arr.index(level["fieldInstances"][0]["__value"])
        exit_dir = dir_arr.index(level["fieldInstances"][1]["__value"])
        
        info_file.write(enter_dir.to_bytes(1,"little"))
        info_file.write(exit_dir.to_bytes(1,"little"))

        if(lvl_width > lvl_height): no_of_secs = math.ceil(lvl_width/64)
        else: no_of_secs = math.ceil(lvl_height/64)

        info_file.write(no_of_secs.to_bytes(1,"little"))
        # info_file.write(spawn_byte_size.to_bytes(2,"little"))

        parallax_factor_x = float(data["defs"]["layers"][3]["parallaxFactorX"])
        parallax_factor_y = float(data["defs"]["layers"][3]["parallaxFactorY"])
        if(parallax_factor_x != 0):
            parallax_factor_x = int(1 / parallax_factor_x)
        else: parallax_factor_x = 1
        if(parallax_factor_y != 0):
            parallax_factor_y = int(1 / parallax_factor_y)
        else: parallax_factor_y = 1

        info_file.write(parallax_factor_x.to_bytes(1,"little"))
        info_file.write(parallax_factor_y.to_bytes(1,"little"))

        layer0 = find_level(level["layerInstances"], "Layer0")
        layer1 = find_level(level["layerInstances"], "Layer1")
        layer1_edit = find_level(level["layerInstances"], "Layer1_Edit")


        entities = level["layerInstances"][0]
        full_filename = filename + "S" + str(i).zfill(2) + extension
        with open(full_filename, "wb") as f:
            f.write((0).to_bytes(2, "little"))
            parse_entities(entities, lvl_width, lvl_height, f)


        for j in range(no_of_secs):
            #create file for layer0 and layer1
            full_filename = filename + "R" + str(i).zfill(2) + "L" + "0" + str(j) + extension
            with open(full_filename, "wb") as f:
                f.write((0).to_bytes(2, "little"))
                parse_level(j, layer0["__tilesetDefUid"], layer0["gridTiles"], lvl_width, lvl_height, f)

            full_filename = filename + "R" + str(i).zfill(2) + "L" + "1" + str(j) + extension
            with open(full_filename, "wb") as f:
                f.write((0).to_bytes(2, "little"))
                tile_arr = parse_level(j, layer1["__tilesetDefUid"], layer1["autoLayerTiles"], lvl_width, lvl_height, f, layer1_edit["gridTiles"], layer1_edit["__tilesetDefUid"])

        full_filename = filename + "C" + str(i).zfill(2) + extension
        with open(full_filename, "wb") as f:
            f.write((0).to_bytes(2, "little"))
            save_collision_to_file(tile_arr, lvl_width, lvl_height, f)
