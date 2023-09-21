require("sprite_tessellator")

-- TODO: automatically capitalize filenames
-- TODO: implement smart and custom sprite splitting
-- TODO: solve flatten layer issue
-- TODO: replace asserts with alerts instead
-- TODO: export frame and animation data
-- TODO: remove relative path saving
function call_exporter()
    local active_spr = app.sprite
    assert(active_spr ~= nil, "A sprite must be selected before a palette can be exported.")

    local plugin_key = "randomcardboardbox/x16_exporter"
    local dialog = Dialog("Export Sprite for X16")

    function split_file_path(filename)
        local str_arr = {""}
        
        local index = 1
        for i=1, #(filename), 1 do
            local char = string.sub(filename, i,i)
            if(char == "/" or char == "\\") then
                index = index + 1
                str_arr[index] = ""
            else
                str_arr[index] = str_arr[index] .. char
            end
        end
        
        return str_arr
    end

    function get_current_path()
        local spr_filepath = split_file_path(active_spr.filename)
        local path = ""
        for i=1, #(spr_filepath)-1, 1 do
            path = path .. spr_filepath[i] .."\\"
        end

        return path
    end

    function get_relative_path(spr_filename, filename)
        local spr_filepath = split_file_path(spr_filename)
        local expr_filepath = split_file_path(filename)
        num_of_sim_paths = 0
        for i=1, #(spr_filepath)-1, 1 do
            if(expr_filepath[i] == spr_filepath[i]) then
                num_of_sim_paths = num_of_sim_paths + 1
            else
                break
            end

        end

        num_of_backs = #(spr_filepath) - num_of_sim_paths - 1
        
        rel_filename = ""
        for i=1, num_of_backs, 1 do
            rel_filename = rel_filename .. "..\\"
        end
        for i=num_of_sim_paths+1, #(expr_filepath), 1 do
            rel_filename = rel_filename .. expr_filepath[i]
            if(i ~= #expr_filepath) then
                rel_filename = rel_filename .. "\\"
            end
        end
        return(rel_filename)
    end

    function parse_palette_data(file)
        pal_data_arr = {}
        palette = active_spr.palettes[1]
        for i=0, #palette-1, 1 do
            local col = palette:getColor(i)
            local first_byte = (col.green & 240) | (col.blue >> 4)
            local second_byte = (col.red >> 4)

            file:write(string.char(first_byte))
            file:write(string.char(second_byte))
        end
    end

    function set_closest_size(size)
        local spr_sizes = {8,16,32,64}
        for i=1, #spr_sizes, 1 do
            if(size <= spr_sizes[i]) then
                return(spr_sizes[i])
            end
        end
    end

    function can_sav_tile(x,y,width,height,cel)
        if(dialog.data.export_empty_tiles == true or (dialog.data.export_empty_tiles == false and 
                (not is_tile_empty({x,y,width,height}, cel.image, 0, 0)))) then
            return(true)
        else
            return(false)
        end
    end 

    function parse_tile_data(file, img)
        if(dialog.data.spr_bpp == "8") then
            for j=0, img.height-1, 1 do
                for i=0, img.width-1, 1 do
                    col_ind = img:getPixel(i,j)
                    file:write(string.char(col_ind))
                end
            end
        elseif(dialog.data.spr_bpp == "4") then
            for j=0, img.height-1, 1 do
                for i=0, (img.width/2)-1, 1 do
                    col_ind1 = img:getPixel(i*2,j)
                    col_ind2 = img:getPixel(i*2+1,j)

                    local byte = (col_ind2%16) | ((col_ind1%16) << 4)
                    file:write(string.char(byte))
                end
            end
        elseif(dialog.data.spr_bpp == "2") then
            for j=0, img.height-1, 1 do
                for i=0, (img.width/4)-1, 1 do
                    col_ind1 = img:getPixel(i*4,j)
                    col_ind2 = img:getPixel(i*4+1,j)
                    col_ind3 = img:getPixel(i*4+2,j)
                    col_ind4 = img:getPixel(i*4+3,j)

                    local byte = (col_ind4%4) | ((col_ind3%4) << 2) | ((col_ind2%4) << 4) | ((col_ind1%4) << 6)
                    file:write(string.char(byte))
                end
            end
        end
    end

    function get_tilemap_ind(img, x,y)
        if(x>=0 and x<img.width and y>=0 and y<img.height) then
            return(img:getPixel(x,y))
        else
            return(0)
        end
    end

    function parse_tilemap_data(file)
        assert(active_spr.colorMode == ColorMode.INDEXED, "Sprite must be in idexed colour mode before it can be exported.")
        
        local layer = active_spr.layers[1]
        local tileset = layer.tileset

        local t_ind = 0
        for i=1, 1000, 1 do
            local tileImage = tileset:getTile(t_ind)
            if(tileImage == nil) then break end
            
            parse_tile_data(file, tileImage)

            t_ind = t_ind+1
        end


        -- export tileset data for python exporter script
        local img_width = active_spr.width / 32
        local img_height = active_spr.height / 32
        local tilemap = layer.cels[1].image

        metatiles = {}

        for j=0, img_width, 1 do
            for i=0, img_height, 1 do
                local ind1 = get_tilemap_ind(tilemap, i*2,j*2)
                local ind2 = get_tilemap_ind(tilemap, i*2+1,j*2)
                local ind3 = get_tilemap_ind(tilemap, i*2,j*2+1)
                local ind4 = get_tilemap_ind(tilemap, i*2+1,j*2+1)

                metatile = {ind1,ind2,ind3,ind4}
                metatiles[#metatiles+1] = metatile
            end
        end

        local file,err = io.open(dialog.data.ts_filename,'wb')
        if file then
            file:write( json.encode(metatiles) )
        else
            print("error:", err)
        end

        file:close()
    end

    function parse_micro_spr_data(file, cel, x, y, width, height)
        local image = cel.image
        
        if(dialog.data.spr_bpp == "8") then
            for j=0, height-1, 1 do
                for i=0, width-1, 1 do
                    local x_pos = (x+i)
                    local y_pos = (y+j)
                    if(x_pos >= 0 and x_pos < cel.bounds.width and 
                            y_pos >= 0 and y_pos < cel.bounds.height) then 
                        col_ind = image:getPixel(x_pos,y_pos) % 256
                    else col_ind = 0 end
                    file:write(string.char(col_ind))
                end
            end
        else
            for j=0, height-1, 1 do
                for i=0, math.ceil(width/2)-1, 1 do
                    local x_pos = (x+i*2)
                    local y_pos = (y+j)
                    if(x_pos >= 0 and x_pos < cel.bounds.width and 
                            y_pos >= 0 and y_pos < cel.bounds.height) then 
                        col_ind1 = image:getPixel(x_pos,y_pos) % 16
                    else col_ind1 = 0 end
                    
                    local x_pos = x_pos + 1 
                    if(x_pos >= 0 and x_pos < cel.bounds.width and 
                            y_pos >= 0 and y_pos < cel.bounds.height) then 
                        col_ind2 = image:getPixel(x_pos,y_pos) % 16
                    else col_ind2 = 0 end
                    
                    local byte = (col_ind2) | (col_ind1 << 4)
                    file:write(string.char(byte))
                end
            end
        end
    end

    function parse_sprite_data(file)
        assert(active_spr.colorMode == ColorMode.INDEXED, "Sprite must be in indexed colour mode before it can be exported.")
        -- app.command.FlattenLayers()

        -- for frame=1, fr
        local layer = active_spr.layers[#active_spr.layers]

        -- tiles are the portions the sprite is split into
        local tile_width = tonumber(dialog.data.spr_width)
        local tile_height = tonumber(dialog.data.spr_height)

        local spr_width = active_spr.width
        local spr_height = active_spr.height

        local rows_first = dialog.data.split_by_rows

        local max_spr_tiles = 0

        local frames = {}
        local mem_addr = 0

        for c_ind=1, #(layer.cels), 1 do
            table.insert(frames, {})
            local num_spr_tiles = 0

            local cel = layer.cels[c_ind]
            if(dialog.data.spr_split_style == "by grid") then
                local curr_spr_x = 0
                local curr_spr_y = 0
                
                local iteration = 0
                while iteration < 100 do
                    iteration = iteration + 1

                    if(rows_first and (curr_spr_y >= spr_height)) then break end
                    if(not rows_first and (curr_spr_x >= spr_width)) then break end
                    
                    local curr_tile_width = math.min(tile_width, active_spr.width-curr_spr_x)
                    local curr_tile_height = math.min(tile_height, active_spr.height-curr_spr_y)

                    curr_tile_width = set_closest_size(curr_tile_width)
                    curr_tile_height = set_closest_size(curr_tile_height)

                    if(can_sav_tile(curr_spr_x, curr_spr_y, curr_tile_width, curr_tile_height, cel)) then
                        if(not dialog.data.export_as_metatile) then
                            parse_micro_spr_data(file, cel, curr_spr_x, curr_spr_y, curr_tile_width, curr_tile_height)
                            num_spr_tiles = num_spr_tiles + 1
                            table.insert(frames[#frames], {curr_spr_x + cel.position.x, curr_spr_y + cel.position.y, curr_tile_width, curr_tile_height, mem_addr})
                            if(dialog.data.spr_bpp == "8") then mem_addr = mem_addr + (curr_tile_width*curr_tile_height)
                            elseif(dialog.data.spr_bpp == "4") then mem_addr = mem_addr + (curr_tile_width*curr_tile_height) /2 
                            else mem_addr = mem_addr + (curr_tile_width*curr_tile_height) /4 end
                        else
                            parse_micro_spr_data(file, cel, curr_spr_x, curr_spr_y, curr_tile_width/2, curr_tile_height/2)
                            parse_micro_spr_data(file, cel, curr_spr_x+(curr_tile_width/2), curr_spr_y, curr_tile_width/2, curr_tile_height/2)
                            parse_micro_spr_data(file, cel, curr_spr_x, curr_spr_y+(curr_tile_height/2), curr_tile_width/2, curr_tile_height/2)
                            parse_micro_spr_data(file, cel, curr_spr_x+(curr_tile_width/2), curr_spr_y+(curr_tile_height/2), curr_tile_width/2, curr_tile_height/2)
                        end
                    end

                    if(rows_first) then
                        curr_spr_x = curr_spr_x + curr_tile_width
                        if(curr_spr_x >= spr_width) then
                            curr_spr_x = 0
                            curr_spr_y = curr_spr_y + curr_tile_height
                        end
                    else
                        curr_spr_y = curr_spr_y + curr_tile_height
                        if(curr_spr_y >= spr_height) then
                            curr_spr_y = 0
                            curr_spr_x = curr_spr_x + curr_tile_width
                        end
                    end

                    -- - if the curr_spr_y is more than the spr_width break out of loop
                    -- - format tile width and height correctly
                    -- - parse that individual tile and pass it to file
                    -- - if diff was less than the tile or equal to tile width or height go to the next row and reset curr_spr_x
                    -- - otherwise simply add the tile_width or tile_height to curr_spr var
                end
            else    
                -- for smart tile division
                local tile_sizes = {8,16,32,64}
                local start_size = 1
                if(dialog.data.spr_width == "8")then start_size = 1 end
                if(dialog.data.spr_width == "16")then start_size = 2 end
                if(dialog.data.spr_width == "32")then start_size = 3 end
                if(dialog.data.spr_width == "64")then start_size = 4 end

                tiles = find_tiles(cel.image, start_size)[2]
                num_spr_tiles = #tiles
                for t_ind=1, #tiles, 1 do
                    tile = tiles[t_ind]
                    parse_micro_spr_data(file, cel, tile[1], tile[2], tile[3], tile[4])

                    table.insert(frames[#frames], {tile[1] + cel.position.x, tile[2] + cel.position.y, tile[3], tile[4], mem_addr})
                    if(dialog.data.spr_bpp == "8") then mem_addr = mem_addr + (tile[3]*tile[4])
                    elseif(dialog.data.spr_bpp == "4") then mem_addr = mem_addr + (tile[3]*tile[4]) /2 
                    else mem_addr = mem_addr + (tile[3]*tile[4]) /4 end
                end
            end

            if(num_spr_tiles > max_spr_tiles) then
                max_spr_tiles = num_spr_tiles
            end
        end
        -- app.command.Undo()
        return({max_spr_tiles, frames})
    end

    function get_spr_size_byte(size)
        if(size == 8) then return(0) end
        if(size == 16) then return(1) end
        if(size == 32) then return(2) end
        if(size == 64) then return(3) end
    end

    function set_sprite_attr_data(tile, bytes, reverse)
        bytes[1] = ((tile[5] >> 5)) % 256
        bytes[2] = (((tile[5] >> 13) & 15)) % 256
        bytes[3] = (tile[1]) % 256
        bytes[4] = (tile[1] >> 8) % 256
        bytes[5] = (tile[2]) % 256
        bytes[6] = (tile[2] >> 8) % 256
        bytes[7] = (12) % 256
        bytes[8] = ((get_spr_size_byte(tile[4]) << 6) | (get_spr_size_byte(tile[3]) << 4)) % 256
        
        if(reverse == true) then bytes[7] = bytes[7] | 1 end
        if(dialog.data.spr_bpp == "8") then byte[2] = (byte[2] | 128) end
    end

    function parse_anim_data(file, num_of_sprites, frames)
        -- export the number of tiles
        -- export each frame
        -- for each frame export each sprite tile)
        file:write(string.char(num_of_sprites))

        local layer = active_spr.layers[#active_spr.layers]
        
        --tile x
        --tile y
        --tile width
        --tile height
        --sprite address
        
        for c_ind=1, #(layer.cels), 1 do
            frame = frames[c_ind]

            local frame_time = math.floor((layer.cels[c_ind].frame.duration * 60))
            file:write(string.char(frame_time % 256))
            file:write(string.char(0))

            -- right facing animation frame
            for spr_ind=1, num_of_sprites, 1 do
                local bytes = {0,0,0,0,0,0,0,0}
                if(frame[spr_ind]) then set_sprite_attr_data(frame[spr_ind], bytes, false) end
                for b_ind=1, 8, 1 do
                    file:write(string.char(bytes[b_ind]))
                end
            end
            --left facing animation frame
            for spr_ind=1, num_of_sprites, 1 do
                local bytes = {0,0,0,0,0,0,0,0}
                if(frame[spr_ind]) then 
                    frame[spr_ind][1] = active_spr.width - frame[spr_ind][1] - frame[spr_ind][3]
                    set_sprite_attr_data(frame[spr_ind], bytes, true) 
                end
                for b_ind=1, 8, 1 do
                    file:write(string.char(bytes[b_ind]))
                end
            end
        end
    end

    function export_palette()
        local filepath = dialog.data.pal_filename
        local file,err = io.open(filepath,'wb')
        if file then
            file:write(string.char(0,0)) -- two byte buffer required for all x16 files
            parse_palette_data(file)
            file:close()
            dialog:close()
        else
            print("error:", err)
        end
    end

    function can_export_anim()
        if(#active_spr.frames > 1) then
            return(true)
        else
            return(false)
        end
    end

    function export_sprite()
        local filepath = dialog.data.filename
        local file,err = io.open(filepath,'wb')
        local file2 = nil
        local err2 = nil
        if(can_export_anim()) then
            file2, err2 = io.open(dialog.data.anim_filename, 'wb')
        end

        if (file and (not can_export_anim() or file2)) then
            file:write(string.char(0,0)) -- two byte buffer required for all x16 files

            if(dialog.data.add_empty_tile) then
                size = tonumber(dialog.data.spr_width) * tonumber(dialog.data.spr_height)
                if(dialog.data.spr_bpp == "8") then
                    for i=1, size, 1 do
                        file:write(string.char(0))
                    end
                end
                if(dialog.data.spr_bpp == "4") then
                    for i=1, size/2, 1 do
                        file:write(string.char(0))
                    end
                end
                if(dialog.data.spr_bpp == "2") then
                    for i=1, size/4, 1 do
                        file:write(string.char(0))
                    end
                end
            end


            if(active_spr.layers[1].isTilemap) then
                parse_tilemap_data(file)
                app.command.Undo()
            else
                return_data = parse_sprite_data(file)
                num_of_sprites = return_data[1]
                frames = return_data[2]

                if(can_export_anim()) then
                    file2:write(string.char(0,0))
                    parse_anim_data(file2, num_of_sprites, frames)
                    file2:close()
                end
            end
            file:close()
            dialog:close()
        else
            print("error 1:", err)
            print("error 2:", err2)
            print("file obj 1:", file)
            print("file obj 2:", file2)
            print("attempted 1:", dialog.data.filename)
            print("attempted 2:", dialog.data.anim_filename)
        end
    end

    function add_confirm_dia()
        local dialog2 = Dialog("Export Complete")
        dialog2:label{
            text="Sprite Saved to '" .. dialog.data.filename .. "'"
        }
        if(dialog.data.pal_exp_check) then
            dialog2:newrow{}
            dialog2:label{
            text="Palette Saved to '" .. dialog.data.pal_filename .. "'"
            }
        end

        dialog2:button{
            id = "ok",
            text = "OK",
            onclick = function()
                dialog2:close()
            end
        }

        dialog2:show{ wait = false }
    end

    dialog:file{
        id = "filename",
        label = "Sprite Export File Name",
        open = false,
        save = true,
        entry = true,
        filetypes = {"SPR"},
        filename = active_spr.properties(plugin_key).filename,
        onchange = function()
            active_spr.properties(plugin_key).filename = dialog.data.filename
        end
    }

    -- dialog:label{text="Size"}
    dialog:label{label="Size:", text="x"}
    dialog:label{text="y"}
    dialog:newrow{}

    dialog:combobox{ 
        id="spr_width",
        option=active_spr.properties(plugin_key).spr_width,
        options={ "8", "16", "32", "64" },
        onchange = function()
            active_spr.properties(plugin_key).spr_width = dialog.data.spr_width
        end
    }

    dialog:combobox{
        id="spr_height",
        option=active_spr.properties(plugin_key).spr_height,
        options={ "8", "16", "32", "64" },
        onchange = function()
            active_spr.properties(plugin_key).spr_height = dialog.data.spr_height
        end
    }

    dialog:combobox{ 
        id="spr_bpp",
        label="Bits per pixel",
        option=active_spr.properties(plugin_key).spr_bpp,
        options={ "2", "4", "8"},
        onchange = function()
            active_spr.properties(plugin_key).spr_bpp = dialog.data.spr_bpp
        end
    }

    dialog:check{ 
        id = "split_by_rows",
        label = "Split sprite by rows first",
        selected = active_spr.properties(plugin_key).split_by_rows,
        onclick = function()
            active_spr.properties(plugin_key).split_by_rows = dialog.data.split_by_rows
        end
    }
    dialog:check{ 
        id = "export_empty_tiles",
        label = "Export Empty Tiles",
        selected = active_spr.properties(plugin_key).export_empty_tiles,
        onclick = function()
            active_spr.properties(plugin_key).export_empty_tiles = dialog.data.export_empty_tiles
        end
    }
    dialog:check{ 
        id = "add_empty_tile",
        label = "Add Empty Tile to Start of File",
        selected = active_spr.properties(plugin_key).add_empty_tile,
        onclick = function()
            active_spr.properties(plugin_key).add_empty_tile = dialog.data.add_empty_tile
        end
    }

    dialog:check{ 
        id = "as_tileset",
        label = "Export as Tileset",
        selected = active_spr.properties(plugin_key).as_tileset,
        onclick = function()
            active_spr.properties(plugin_key).as_tileset = dialog.data.as_tileset
        end
    }

    dialog:check{
        id="export_as_metatile",
        label = "Export as Metatile",
        selected=active_spr.properties(plugin_key).export_as_metatile,
        onclick = function()
            active_spr.properties(plugin_key).export_as_metatile = dialog.data.export_as_metatile
        end
    }

    dialog:combobox{
        id="spr_split_style",
        label = "Sprite split option",
        option=active_spr.properties(plugin_key).spr_split_style,
        options={ "by grid", "smart", "custom"},
        onchange = function()
            active_spr.properties(plugin_key).spr_split_style = dialog.data.spr_split_style
        end
    }

    dialog:separator{ 
        id="sep",
        text="" 
    }

    dialog:check{ 
        id = "pal_exp_check",
        label = "Export Palette",
        selected = active_spr.properties(plugin_key).exp_pal,
        onclick = function()
            active_spr.properties(plugin_key).exp_pal = dialog.data.pal_exp_check
            dialog:modify{ id="pal_filename",
                visible=true,
                enabled=dialog.data.pal_exp_check}
        end
    }

    dialog:file{
        id = "pal_filename",
        label = "Palette Export File Name",
        open = false,
        save = true,
        entry = true,
        filetypes = {"PAL"},
        filename = active_spr.properties(plugin_key).pal_filename,
        onchange = function()
            active_spr.properties(plugin_key).pal_filename = dialog.data.pal_filename
        end
    }

    dialog:file{
        id = "anim_filename",
        label = "Animation Export File Name",
        open = false,
        save = true,
        entry = true,
        filetypes = {"ANM"},
        filename = active_spr.properties(plugin_key).anim_filename,
        onchange = function()
            active_spr.properties(plugin_key).anim_filename = dialog.data.anim_filename
        end
    }

    dialog:file{
        id = "ts_filename",
        label = "Tileset Export File Name",
        open = false,
        save = true,
        entry = true,
        filetypes = {"json"},
        filename = active_spr.properties(plugin_key).ts_filename,
        onchange = function()
            active_spr.properties(plugin_key).ts_filename = dialog.data.ts_filename
        end
    }

    dialog:button{
        id = "export",
        text = "Export",
        onclick = function()
            export_sprite()
            if(dialog.data.pal_exp_check) then
                export_palette()
            end
            add_confirm_dia()
        end
    }

    dialog:button{
        id = "cancel",
        text = "Cancel",
        onclick = function()
            dialog:close()
        end
    }
    -- dialog.bounds = Rectangle(100, 100, bnds.width+200, bnds.height)
    dialog:modify{ id="pal_filename",
                visible=true,
                enabled=dialog.data.pal_exp_check}

    dialog:show{ wait = false }

    bnds = dialog.bounds
    dialog.bounds = Rectangle(bnds.x-50, bnds.y, bnds.width+100, bnds.height)

    dialog:show{ wait = true }
end

function init(plugin)
    plugin:newCommand{
        id="ExportSprite",
        title="Export to X16",
        group="file_export",
        onclick=call_exporter,
    }

end