import math
import numpy

import pygame
from pygame.locals import *

pixel_scale = 10
offset = (40,40)

tile_sizes = [8,16,32,64]
tile_sizes_rev = [64,32,16,8]
tile_sequences = []
new_ts_arr = []
for tile_size in tile_sizes_rev:
    new_ts_arr.append(tile_size)
    for size_ind in range(len(new_ts_arr)):
        size_1 = new_ts_arr[size_ind]
        if((size_1, size_1) not in tile_sequences): tile_sequences.append((size_1, size_1))
        for new_ind in range(len(new_ts_arr)-1,-1,-1):
            size_2 = new_ts_arr[new_ind]
            if((size_2, size_1) not in tile_sequences): tile_sequences.append((size_2, size_1)) 
            if((size_1, size_2) not in tile_sequences): tile_sequences.append((size_1, size_2))

print(tile_sequences)

screen = pygame.display.set_mode((800,800))

def draw_sprite(spr:pygame.Surface):
    scaled_x = spr.get_width()*pixel_scale
    scaled_y = spr.get_height()*pixel_scale
    scaled_spr = pygame.transform.scale(spr, (scaled_x, scaled_y))
    screen.blit(scaled_spr, offset)

    pygame.draw.rect(screen, (255,0,0), (offset[0], offset[1], scaled_x, scaled_y), 5)

def draw_tiles(tiles, tile_groups):
    colours = [
        (210, 15, 57),
        (254, 100, 11),
        (64, 160, 43),
        (23, 146, 153),
        (30, 102, 245),
        (114, 135, 253),
        (230, 69, 83),
        (234, 118, 203),
        (230, 69, 83),
        (223, 142, 29),
        (32, 159, 181)
    ]
    # for tile in tiles:
    #     rect = [tile[0]*pixel_scale+offset[0],
    #             tile[1]*pixel_scale+offset[1],
    #             tile[2]*pixel_scale,
    #             tile[3]*pixel_scale]
    #     pygame.draw.rect(screen, (0,255,0), rect, 2)

    for tile in tile_groups:
        rect = [tile[0]*pixel_scale+offset[0],
                tile[1]*pixel_scale+offset[1],
                tile[2]*pixel_scale,
                tile[3]*pixel_scale]
        pygame.draw.rect(screen, (0,0,255), rect, 2)

def is_transparent(x, y, spr):
    if(x >= 0 and x < spr.get_width() and
            y >= 0 and y < spr.get_height()):
        return(spr.get_at((x,y))[3] == 0)
    
    return(True)

def crop_spr(spr:pygame.Surface):
    min_x = spr.get_width()
    min_y = spr.get_height()
    max_x = 0
    max_y = 0

    for j in range(spr.get_height()):
        for i in range(spr.get_width()):
            if(not is_transparent(i,j,spr)):
                if(i < min_x): min_x = i
                if(i > max_x): max_x = i
                if(j < min_y): min_y = j
                if(j > max_y): max_y = j

    spr_size_x = max_x - min_x + 1
    spr_size_y = max_y - min_y + 1
    new_spr = spr.subsurface(min_x, min_y, spr_size_x, spr_size_y)

    return(new_spr)

def is_tile_empty(tile, spr, offset=(0,0)):
    for j in range(tile[3]):
        for i in range(tile[2]):
            x = i+tile[0] + offset[0]
            y = j+tile[1] + offset[1]
            if(not is_transparent(x,y,spr)):
                return(False)
            
    return(True)

def num_empty_tiles(tiles, spr, offset=(0,0)):
    num = 0
    for tile in tiles:
        if(is_tile_empty(tile, spr, offset)):
            num += 1
    return(num)

def is_opaque(x,y,spr):
    if(x >= 0 and x < len(spr[0]) and
            y >= 0 and y < len(spr)):
        return(spr[y][x] == 1)
    
    return(False)

def is_tile_full(tile, spr, offset=(0,0)):
    for j in range(tile[3]):
        for i in range(tile[2]):
            x = i+tile[0] + offset[0]
            y = j+tile[1] + offset[1]
            if(not is_opaque(x,y,spr)):
                return(False)
            
    return(True)

def num_full_tiles(tiles, spr, offset=(0,0)):
    num = 0
    for tile in tiles:
        if(is_tile_full(tile, spr, offset)):
            num += 1
    return(num)

def all_pix_covered(tiles, spr:pygame.Surface, offset=(0,0)):
    pass
    # pixels = []
    # for j in range(spr.get_height()):
    #     for i in range(spr.get_width()):
    #         if(not is_transparent(spr.get_at(i,j))):
    #             pixels.append(i,j)

    # return(len(pixels) == 0)


def check_tile_perim(tile, spr:pygame.Surface, dir:str, offset=(0,0)): # direct can be "left", "right", "down", "up"
    num_of_pixels = 0
    if(dir=="right"):
        val_range = range(tile[1], tile[1]+tile[3])
        for i in val_range:
            pos = (tile[0]+tile[2]-1+offset[0], i+offset[1])
            if(not is_transparent(pos[0],pos[1],spr)):
                num_of_pixels += 1

    if(dir=="left"):
        val_range = range(tile[1], tile[1]+tile[3])
        for i in val_range:
            pos = (tile[0]+offset[0], i+offset[1])
            if(not is_transparent(pos[0],pos[1],spr)):
                num_of_pixels += 1

    if(dir=="down"):
        val_range = range(tile[0], tile[0]+tile[2])
        for i in val_range:
            pos = (i+offset[0], tile[1]+tile[3]-1+offset[1])
            if(not is_transparent(pos[0],pos[1],spr)):
                num_of_pixels += 1

    if(dir=="up"):
        val_range = range(tile[0], tile[0]+tile[2])
        for i in val_range:
            pos = (i+offset[0], tile[1]+offset[1])
            if(not is_transparent(pos[0],pos[1],spr)):
                num_of_pixels += 1

    return num_of_pixels

def remove_empty_space(tiles, spr, offset):
    for t_ind in range(len(tiles)-1, -1, -1):
        tile = tiles[t_ind]
        if(is_tile_empty(tile, spr, offset=offset)):
            del tiles[t_ind]
            continue
        tile[0] += offset[0]
        tile[1] += offset[1]

def remove_full_space(tiles, spr, offset):
    for t_ind in range(len(tiles)-1, -1, -1):
        tile = tiles[t_ind]
        if(not is_tile_full(tile, spr, offset=offset)):
            del tiles[t_ind]
            continue
        tile[0] += offset[0]
        tile[1] += offset[1]

def check_for_split_ver(group, spr, offset=(0,0)):
    last_tile = group[0]
    split_pos = None
    for i in range(1, len(group)):
        if(check_tile_perim(group[i], spr, "up", offset) == 0 or 
           check_tile_perim(last_tile, spr, "down", offset)):
            split_pos = i
            break
        last_tile = group[i]

    return(split_pos)

def check_for_split_hor(group, spr, offset=(0,0)):
    last_tile = group[0]
    split_pos = None
    for i in range(1, len(group)):
        if(check_tile_perim(group[i], spr, "left", offset) == 0 or 
           check_tile_perim(last_tile, spr, "right", offset)):
            split_pos = i
            break
        last_tile = group[i]

    return(split_pos)

def shift_group_ver(group, spr, groups, is_split=False):
    max_empty_tiles = 0
    max_tile_offset = 0
    split = None
    for offset in range(tile_sizes[0]-1):
        if(check_tile_perim(group[0], spr, "up",offset=(0,offset-1))):
            break
        num_em_tiles = num_empty_tiles(group, spr, offset=(0,offset))
        if(num_em_tiles > max_empty_tiles):
            max_empty_tiles = num_em_tiles
            max_tile_offset = offset

        #check for tile split
        if(split is None):
            split = check_for_split_ver(group, spr, (0,offset))

    for offset in range(0, -(tile_sizes[0]-1), -1):
        if(check_tile_perim(group[-1], spr, "down",offset=(0,offset+1))):
            break
        num_em_tiles = num_empty_tiles(group, spr, offset=(0, offset))
        if(num_em_tiles > max_empty_tiles):
            max_empty_tiles = num_em_tiles
            max_tile_offset = offset

        #check for tile split
        if(split is None):
            split = check_for_split_ver(group, spr, (0,offset))
    
    if(not is_split and split is not None):
        group1 = group[0:split].copy()
        group2 = group[split:len(group)].copy()

        split_em1 = shift_group_ver(group1, spr, groups, is_split=True)
        split_em2 = shift_group_ver(group2, spr, groups, is_split=True)

        split_em = split_em1 + split_em2
        if(split_em > max_empty_tiles):
            #replace group with the two smaller groups
            ind = groups.index(group)
            del group
            groups.insert(ind, group1)
            groups.insert(ind, group2)
        
            return(split_em)
    
    remove_empty_space(group, spr, (0, max_tile_offset))
    return(max_empty_tiles)

def shift_group_hor(group, spr, groups):
    max_empty_tiles = 0
    max_tile_offset = 0
    split = None
    for offset in range(1, tile_sizes[0]-1):
        if(check_tile_perim(group[0], spr, "left",offset=(offset-1,0))):
            break
        num_em_tiles = num_empty_tiles(group, spr, offset=(offset,0))
        if(num_em_tiles > max_empty_tiles):
            max_empty_tiles = num_em_tiles
            max_tile_offset = offset

        #check for tile split
        if(split is None):
            split = check_for_split_hor(group, spr, (offset,0))

    for offset in range(-1, -(tile_sizes[0]), -1):
        if(check_tile_perim(group[-1], spr, "right",offset=(offset+1,0))):
            break
        num_em_tiles = num_empty_tiles(group, spr, offset=(offset,0))
        if(num_em_tiles > max_empty_tiles):
            max_empty_tiles = num_em_tiles
            max_tile_offset = offset

        #check for tile split
        if(split is None):
            split = check_for_split_hor(group, spr, (offset,0))

    if(split is not None):
        group1 = group[:split].copy()
        group2 = group[split:].copy()

        split_em1 = shift_group_hor(group1, spr, groups)
        split_em2 = shift_group_hor(group2, spr, groups)

        split_em = split_em1 + split_em2
        if(split_em > max_empty_tiles):
            #replace group with the two smaller groups
            ind = groups.index(group)
            del group
            groups.insert(ind, group1)
            groups.insert(ind, group2)
    
            return(split_em)
    
    remove_empty_space(group, spr, (max_tile_offset, 0))
    return(max_empty_tiles)

def sort_tiles_ver(tiles, spr):
    tile_groups = [[]]
    start_x = tiles[0][0]
    start_y = tiles[0][1]
    last_tile = None
    for tile in tiles:
        if(tile[0] != start_x):
            tile_groups.append([])
            start_x = tile[0]
            start_y = tile[1]

        elif(tile[1] != start_y):
            tile_groups.append([])

        last_tile = tile
        start_y = tile[1] + tile_sizes[0]
        tile_groups[len(tile_groups)-1].append(tile)

    return(tile_groups)

def sort_tiles_hor(tiles, spr):
    tile_groups = [[]]
    start_x = tiles[0][0]
    start_y = tiles[0][1]
    last_tile = None
    for tile in tiles:
        if(tile[1] != start_y):
            tile_groups.append([])
            start_x = tile[0]
            start_y = tile[1]

        elif(tile[0] != start_x):
            tile_groups.append([])

        last_tile = tile
        start_x = tile[0] + tile_sizes[0]
        tile_groups[len(tile_groups)-1].append(tile)

    return(tile_groups)

def edit_tile_map(tiles, spr):
    for j in range(tiles[1], tiles[1]+tiles[3]):
        for i in range(tiles[0], tiles[0]+tiles[2]):
            spr[j][i] = 0

def find_tiles(spr:pygame.Surface):
    spr = crop_spr(spr)
    tiles = []

    spr_width = spr.get_width()
    spr_height = spr.get_height()

    # create the initial tile grid
    grid_size = (math.ceil(spr_width/tile_sizes[0]),
                 math.ceil(spr_height/tile_sizes[0]))
    for i in range(grid_size[0]):
        for j in range(grid_size[1]):
            new_tile = [
                i*tile_sizes[0],
                j*tile_sizes[0],
                tile_sizes[0],
                tile_sizes[0],
            ]
            tiles.append(new_tile)

    # going through all possible permutabtions of grids and 
    # checking which one is the most space efficient
    max_empty_tiles = 0
    max_tile_offsets = [(0,0)]

    offset_range_x = min(tile_sizes[0]-1, (grid_size[0]*tile_sizes[0]) - spr_width)
    offset_range_y = min(tile_sizes[0]-1, (grid_size[1]*tile_sizes[0]) - spr_height)

    for j in range(0, -offset_range_y, -1):
        for i in range(0, -offset_range_x, -1):
            num_em_tiles = num_empty_tiles(tiles, spr, offset=(i,j))
            if(num_em_tiles == max_empty_tiles):
                max_tile_offsets.append((i,j))
            if(num_em_tiles > max_empty_tiles):
                max_empty_tiles = num_em_tiles
                max_tile_offsets = []
                max_tile_offsets.append((i,j))

    #update tile positions with new tile offset
    tile_offset = max_tile_offsets[0]

    print(offset_range_x)
    print(offset_range_y)

    print(tile_offset)

    remove_empty_space(tiles, spr, tile_offset)

    tile_2d_arr = []
    for j in range(grid_size[1]):
        tile_2d_arr.append([])
        for i in range(grid_size[0]):
            tile_2d_arr[j].append(0)

    for tile in tiles:
        x_pos = int((tile[0] - tile_offset[0]) / tile_sizes[0])
        y_pos = int((tile[1] - tile_offset[1]) / tile_sizes[0])

        tile_2d_arr[y_pos][x_pos] = 1

    group_tiles = []

    # start implementing here
    curr_tile_size = (tile_sizes[0], tile_sizes[0])
    for k in range(len(tile_sequences)):
        meta_tile_size = tile_sequences[k]
        tile_ratio = (int(meta_tile_size[0] / curr_tile_size[0]),
                    int(meta_tile_size[1] / curr_tile_size[1]))
        
        meta_grid_size = (
            math.floor(grid_size[0]*(curr_tile_size[0] / meta_tile_size[0])),
            math.floor(grid_size[1]*(curr_tile_size[1] / meta_tile_size[1]))
        )
        
        meta_tile_arr = []
        for j in range(meta_grid_size[1]):
            for i in range(meta_grid_size[0]):
                meta_tile_arr.append([
                    i*tile_ratio[0],
                    j*tile_ratio[1],
                    tile_ratio[0],
                    tile_ratio[1]
                ])

        offset_range_x = int(min((meta_tile_size[0] / curr_tile_size[0])-1, grid_size[0] - (meta_tile_size[0] / curr_tile_size[0])))
        offset_range_y = int(min((meta_tile_size[1] / curr_tile_size[1])-1, grid_size[1] - (meta_tile_size[1] / curr_tile_size[1])))

        max_tile_offset = (0,0)
        max_full_tiles = 0
        for j in range(offset_range_y+1):
            for i in range(offset_range_x+1):
                num_f_tiles = num_full_tiles(meta_tile_arr, tile_2d_arr, offset=(i,j))
                if(num_f_tiles > max_full_tiles):
                    max_full_tiles = num_f_tiles
                    max_tile_offset = (i,j)

        remove_full_space(meta_tile_arr, tile_2d_arr, max_tile_offset)

        for meta_tile in meta_tile_arr:
            edit_tile_map(meta_tile, tile_2d_arr)
            group_tiles.append([
                meta_tile[0]*tile_sizes[0]+tile_offset[0],
                meta_tile[1]*tile_sizes[0]+tile_offset[1],
                tile_ratio[0]*tile_sizes[0],
                tile_ratio[1]*tile_sizes[0]
            ])
            
    return([spr, tiles, group_tiles])

# no_of_tiles = 0
# for i in range(1,16):
#     spr = pygame.image.load(f"assets\spr_anim{i}.png").convert_alpha()
#     spr, tiles, tile_groups = find_tiles(spr)
    
#     no_of_tiles += len(tiles)
#     # for group in tile_groups:
#     #     no_of_tiles += len(group)

# no_of_pixels = no_of_tiles * 8 * 8
# max_no_of_pixels = 15 * 64 * 64
# print(f"{(no_of_pixels / max_no_of_pixels)*100}%")
spr = pygame.image.load(f"assets\spr_anim10.png").convert_alpha()
spr, tiles, tile_groups = find_tiles(spr)

running = True
while(running):
    events = pygame.event.get()
    for event in events:
        if(event.type == QUIT):
            running = False


    screen.fill((255,255,255))

    draw_sprite(spr)
    draw_tiles(tiles, tile_groups)

    pygame.display.update()