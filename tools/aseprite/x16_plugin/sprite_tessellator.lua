function is_transparent(x,y,spr) 
    if(x >= 0 and x < spr.width and
    y >= 0 and y < spr.height) then
        local pix = spr:getPixel(x,y)
        if(spr.colorMode == ColorMode.RGB) then
            return(app.pixelColor.rgbaA(pix) == 0)
        end

        if(spr.colorMode == ColorMode.GRAY) then
            return(app.pixelColor.grayaA(pix) == 0)
        end

        if(spr.colorMode == ColorMode.INDEXED) then
            return(pix == 0)
        end
    end
    
    return(true)
end

function is_tile_empty(tile, spr, offset_x, offset_y)
    for j=0, tile[4]-1, 1 do
        for i=0, tile[3]-1, 1 do
            local x = i+tile[1] + offset_x
            local y = j+tile[2] + offset_y
            if(not is_transparent(x,y,spr)) then
                return(false)
            end
        end
    end
            
    return(true)
end

function num_empty_tiles(tiles, spr, offset_x, offset_y)
    local num = 0
    for t_ind=1, #tiles, 1 do
        tile = tiles[t_ind]
        if(is_tile_empty(tile, spr, offset_x, offset_y)) then
            num = num + 1
        end
    end
    return(num)
end

function remove_empty_space(tiles, spr, offset_x, offset_y)
    for t_ind=#tiles, 1, -1 do
        tile = tiles[t_ind]
        if(is_tile_empty(tile, spr, offset_x, offset_y)) then
            table.remove(tiles, t_ind)
            goto continue
        end
        tile[1] = tile[1] + offset_x
        tile[2] = tile[2] + offset_y

        ::continue::
    end
end

function is_opaque(x,y,spr) 
    if(x >= 0 and x < #spr[1] and
            y >= 0 and y < #spr) then
        return(spr[y+1][x+1] == 1)
    end
    
    return(false)
end

function is_tile_full(tile, spr, offset_x, offset_y)
    for j=0, tile[4]-1, 1 do
        for i=0, tile[3]-1, 1 do
            local x = i+tile[1] + offset_x
            local y = j+tile[2] + offset_y
            if(not is_opaque(x,y,spr)) then
                return(false)
            end
        end
    end
    return true
end

function num_full_tiles(tiles, spr, offset_x, offset_y)
    local num=0
    for t_ind=1, #tiles, 1 do
        tile = tiles[t_ind]
        if(is_tile_full(tile, spr, offset_x, offset_y)) then
            num = num + 1
        end
    end
    return(num)
end

function remove_full_space(tiles, spr, offset_x, offset_y)
    for t_ind=#tiles, 1, -1 do
        tile = tiles[t_ind]
        if(not is_tile_full(tile, spr, offset_x, offset_y)) then
            table.remove(tiles, t_ind)
            goto continue
        end
        tile[1] = tile[1] + offset_x
        tile[2] = tile[2] + offset_y

        ::continue::
    end
end

function edit_tile_map(tiles, spr)
    for j=tiles[2], tiles[2]+tiles[4]-1, 1 do
        for i=tiles[1], tiles[1]+tiles[3]-1, 1 do
            spr[j+1][i+1] = 0
        end
    end
end

function find_tiles(spr)
    local tile_sizes = {8,16,32,64}
    local tile_sequences = {{64, 64}, {64, 32}, {32, 64}, {32, 32}, 
                        {64, 16}, {16, 64}, {32, 16}, {16, 32}, 
                        {16, 16}, {64, 8}, {8, 64}, {32, 8}, 
                        {8, 32}, {16, 8}, {8, 16}, {8, 8}}

    local tiles = {}

    local grid_size_x = math.ceil(spr.width/tile_sizes[1])
    local grid_size_y = math.ceil(spr.height/tile_sizes[1])

    for i=0, grid_size_x-1, 1 do
        for j=0, grid_size_y-1, 1 do
            new_tile = {
                i*tile_sizes[1],
                j*tile_sizes[1],
                tile_sizes[1],
                tile_sizes[1],
            }

            table.insert(tiles, new_tile)
        end
    end

    local max_empty_tiles = 0
    local max_tile_offset_x = 0
    local max_tile_offset_y = 0

    local offset_range_x = math.min(tile_sizes[1]-1, (grid_size_x*tile_sizes[1]) - spr.width - 1)
    local offset_range_y = math.min(tile_sizes[1]-1, (grid_size_y*tile_sizes[1]) - spr.height - 1)

    for j=0, -offset_range_y+1, -1 do
        for i=0, -offset_range_x+1, -1 do
            num_em_tiles = num_empty_tiles(tiles, spr, i, j)
            if(num_em_tiles > max_empty_tiles) then
                max_empty_tiles = num_em_tiles
                max_tile_offset_x = i
                max_tile_offset_y = j
            end
        end
    end
    
    remove_empty_space(tiles, spr, max_tile_offset_x, max_tile_offset_y)

    -- meta tiles
    local meta_tiles = {}
    local tile_2d_arr = {}
    for j=1, grid_size_y, 1 do
        table.insert(tile_2d_arr, {})
        for i=1, grid_size_x, 1 do
            table.insert(tile_2d_arr[j], 0)
        end
    end

    for t_ind=1, #tiles, 1 do
        local tile = tiles[t_ind]
        x_pos = math.floor((tile[1] - max_tile_offset_x) / tile_sizes[1])
        y_pos = math.floor((tile[2] - max_tile_offset_y) / tile_sizes[1])

        tile_2d_arr[y_pos+1][x_pos+1] = 1
    end

    for k=1, #tile_sequences, 1 do
        local meta_tile_size_x = tile_sequences[k][1]
        local meta_tile_size_y = tile_sequences[k][2]

        local tile_ratio_x = math.floor(meta_tile_size_x / tile_sizes[1])
        local tile_ratio_y = math.floor(meta_tile_size_y / tile_sizes[1])

        local meta_grid_size_x = math.floor(grid_size_x*(tile_sizes[1] / meta_tile_size_x))
        local meta_grid_size_y = math.floor(grid_size_y*(tile_sizes[1] / meta_tile_size_y))

        local meta_tile_arr = {}
        for j=0, meta_grid_size_y-1, 1 do
            for i=0, meta_grid_size_x-1, 1 do
                table.insert(meta_tile_arr, {
                        i*tile_ratio_x,
                        j*tile_ratio_y,
                        tile_ratio_x,
                        tile_ratio_y})
            end
        end

        local offset_range_x = math.floor(math.min((meta_tile_size_x / tile_sizes[1])-1, grid_size_x - (meta_tile_size_x / tile_sizes[1])))
        local offset_range_y = math.floor(math.min((meta_tile_size_y / tile_sizes[1])-1, grid_size_y - (meta_tile_size_y / tile_sizes[1])))

        local meta_tile_offset_x = 0
        local meta_tile_offset_y = 0
        local max_full_tiles = 0
        
        for j=0, offset_range_y, 1 do
            for i=0, offset_range_x, 1 do
                num_f_tiles = num_full_tiles(meta_tile_arr, tile_2d_arr, i,j)
                if(num_f_tiles > max_full_tiles) then
                    max_full_tiles = num_f_tiles
                    meta_tile_offset_x = i
                    meta_tile_offset_y = j
                end
            end
        end
        
        remove_full_space(meta_tile_arr, tile_2d_arr, meta_tile_offset_x, meta_tile_offset_y)


        for mt_ind=1, #meta_tile_arr, 1 do
            meta_tile = meta_tile_arr[mt_ind]
            edit_tile_map(meta_tile, tile_2d_arr)

            table.insert(meta_tiles, {
                meta_tile[1]*tile_sizes[1]+max_tile_offset_x,
                meta_tile[2]*tile_sizes[1]+max_tile_offset_y,
                tile_ratio_x*tile_sizes[1],
                tile_ratio_y*tile_sizes[1]
            })
        end
    end

    return({tiles, meta_tiles})
end