require("sprite_tessellator")

local listener = nil
local listener2 = nil
local dialog = Dialog{
    title="Sprite Tessellation",
    onclose=function()    
        if(listener) then app.events:off(listener) end
        if(listener2 and app.sprite) then app.sprite.events:off(listener2) end
    end}

local image = nil
local tiles = nil
local meta_tiles = nil
local curr_frame = 0

function get_image()
    local frame_num = app.frame.frameNumber
    local layer = app.frame.sprite.layers[#app.frame.sprite.layers]
    local cel = layer.cels[frame_num]
    local new_image = cel.image
    image = new_image
    return(new_image)
end

function init(plugin)
    plugin:newCommand{
      id="RefreshSpriteTiles",
      title="Refresh Sprite Tesellation",
      group="view_screen",
      onclick=function()
        dialog:show{wait = false}
        if(app.frame) then
            update_image()
        end
        listener = app.events:on('sitechange', function()
            if(app.sprite) then
                if(listener2) then app.sprite.events:off(listener2) end
                listener2 = app.sprite.events:on("change", function()
                    update_image()
                end)
            end
            update_image()
        end)
        if(app.sprite) then
            listener2 = app.sprite.events:on("change", function()
                update_image()
            end)
        end
        update_image()
      end
    }
end

-- function updata_text()
--     local max_size = ((app.sprite.width * app.sprite.height) / 2)-- * #app.sprite.frames
--     local curr_size = (8*8 * #tiles) / 2

--     local percent = (curr_size/max_size) * 100
--     dialog:modify{
--         id="reduction_label",
--         text="Reduction: " .. tostring(100-percent) .. "%"
--     }
-- end

function update_image() 
    if(app.frame) then
        local new_img = get_image()
        tile_arr = find_tiles(image)
        tiles = tile_arr[1]
        meta_tiles = tile_arr[2]
        curr_frame = app.frame.frameNumber
    end

    dialog:repaint()
end

function draw_tiles(gc, x, y, scale)
    gc:beginPath()
    for t_ind=1, #tiles, 1 do
        tile = tiles[t_ind]
        local rect1 = Rectangle((tile[1]*scale)+x,
                                (tile[2]*scale)+y,
                                tile[3]*scale,
                                tile[4]*scale)
        gc:rect(rect1)
    end
    gc.color = Color(0,0,255)
    gc.strokeWidth = 2
    gc:stroke()

    gc:beginPath()
    for t_ind=1, #meta_tiles, 1 do
        tile = meta_tiles[t_ind]
        local rect1 = Rectangle((tile[1]*scale)+x,
                                (tile[2]*scale)+y,
                                tile[3]*scale,
                                tile[4]*scale)
        gc:rect(rect1)
    end
    gc.color = Color(255,0,0)
    gc.strokeWidth = 2
    gc:stroke()
end

function draw_sprite(gc) 
    local width = gc.width
    local height = gc.height

    local w_ratio_w = width/height
    local w_ratio_i = image.width/image.height
    local h_ratio_i = image.height/image.width

    local img_width = 200
    local img_height = 200

    if(w_ratio_i > w_ratio_w) then
        img_width = width * 0.8
        img_height = (h_ratio_i) * img_width
    else
        img_height = height * 0.8
        img_width = (w_ratio_i) * img_height
    end

    local x = (width - img_width) / 2
    local y = (height - img_height) / 2

    gc:drawImage(image,0,0,image.width,image.height,
                    x,y,img_width,img_height)

    draw_tiles(gc, x, y, (img_width/image.width))
end

-- dialog:label{
--     id="reduction_label",
--     text="Reduction: 0%"
-- }

dialog:canvas{
    hexpand = true,
    vexpand = true,
    onkeydown = function(ev)
        if(ev.code == "KeyO") then
            update_image()
        end
    end,
    onpaint = function(ev)
        if(app.frame and image ~= nil) then
            gc = ev.context

            draw_sprite(gc)
        end
    end
}

listener = app.events:on('sitechange', function()
    if(app.sprite) then
        if(listener2) then app.sprite.events:off(listener2) end
        listener2 = app.sprite.events:on("change", function()
            update_image()
        end)
        update_image()
    end
end)
if(app.sprite) then
    listener2 = app.sprite.events:on("change", function()
        update_image()
    end)
end
update_image()
dialog:show{ wait = false }