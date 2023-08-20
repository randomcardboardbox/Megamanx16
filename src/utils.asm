.include "x16.inc"
.include "zeropage.inc"

.export __load_palette_from_file
.export __load_file_into_vram
.export __load_file_into_ram
.export __set_sprite_attribute
.export __clear_sprite_attribute
.export __offset_spr_position
.export __set_layer0_enable
.export __set_layer1_enable
.export __set_sprites_enable
.export __load_vert_map_sect
.export __check_collision_data
.export __transfer_ram_to_vram
.export __transfer_spr_attr_to_vram
.export __wait_for_nmi
.export __get_joystick_state
.export __init_irq_handler
.export __force_halt

; TDONE:load file into ram
; TDONE:load file into vera
; TDONE:load palette function
; TDONE:set sprite sttribute
; TODO:set layer data
; TODO:set video setting data
; TODO:transfer vram to ram
; TODO:transfer ram to vram

; _load_palette_from_file(int fname_ptr, char fname_size, char pal_off):
; fname_ptr: filename pointer - a pointer to the filename string
; fname_size: filename size - the size of the filename in bytes
; pal_off: palette offset - (must be from 0-15) the offset in the palette table 
;                           where the palette file will be loaded

__load_palette_from_file:
    ; storing the variables loaded
    ; by pushing them onto the stack
    pha

    lda (sp)
    inc sp
    pha 
    
    lda (sp)
    inc sp
    pha

    lda (sp)
    inc sp
    pha

    ; reading the file into vram
    lda #1
    ldx #8
    ldy #0
    jsr SETLFS

    ply ; pulling filename start addr
    plx ; off of the stack
    pla ; pulling filename size off stack
    jsr SETNAM
    
    pla ; pulling the palette offset off stack
    asl ; shfiting the bits right to
    asl ; multiply the number of 32
    asl ; (size in bytes of 1 palette block)
    asl
    asl

    ; adding this value to palette address in vram
    clc 
    adc #<VRAM_palette
    tax

    ; setting high byte of vram palette accounting for
    ; carry from last operation
    lda #>VRAM_palette
    adc #0
    tay

    lda #3
    jsr LOAD

    rts

; _load_file_into_vram(int fname_ptr, char fname_size, int ram_addr):
; fname_ptr: filename pointer - a pointer to the filename string
; fname_size: filename size - the size of the filename in bytes
; ram_addr: ram addreass - the location where the data is actually loaded
__load_file_into_ram:
    pha
    phx

    lda (sp)
    inc sp
    pha

    lda (sp)
    inc sp
    pha

    lda (sp)
    inc sp
    pha


    lda #1
    ldx #8
    ldy #0
    jsr SETLFS

    ply ; pulling filename start addr
    plx ; off of the stack
    pla ; pulling filename size off stack
    jsr SETNAM

    lda #0
    ply
    plx
    jsr LOAD

    rts

; _load_file_into_vram(int fname_ptr, char fname_size, char vram_bank, int vram_addr):
; fname_ptr: filename pointer - a pointer to the filename string
; fname_size: filename size - the size of the filename in bytes
; vram_bank: vram bank - (must be from 0-1) the bank to load the data into
; vram_addr: vram addreass - the location where the data is actually loaded
__load_file_into_vram:
    ; storing the variables loaded
    ; by pushing them onto the stack
    pha 
    phx

    lda (sp)
    inc sp
    pha

    lda (sp)
    inc sp
    pha

    lda (sp)
    inc sp
    pha

    lda (sp)
    inc sp
    pha

    ; reading the file into vram
    lda #1
    ldx #8
    ldy #0
    jsr SETLFS

    ply ; pulling filename start addr
    plx ; off of the stack
    pla ; pulling filename size off stack
    jsr SETNAM

    pla 
    clc
    adc #02
    ply
    plx
    jsr LOAD

    rts

; _set_sprite_attribute(char attr_num, int addr, char bpp_mode, int x, int y, char spr_attr, char size, char pal_off)
__set_sprite_attribute:
    pha
    stz VERA_ctrl
    ldy #9
    lda (sp), y
    asl
    asl
    asl
    clc
    adc #<VRAM_sprattr
    sta VERA_addr_low
    
    lda #0
    adc #>(VRAM_sprattr)
    sta VERA_addr_high

    lda #%00010001
    sta VERA_addr_bank

    ldy #7
    lda (sp), y
    lsr
    sta VERA_data0


    ldy #6
    lda (sp), y
    asl
    asl
    asl
    asl
    asl
    asl
    asl
    sta ZP_PTR_1

    iny
    lda (sp),y
    lsr
    ora ZP_PTR_1
    sta VERA_data0

    ; setting x and y position of sprite 
    ldy #4
    lda (sp),y
    sta VERA_data0
    iny
    lda (sp),y
    sta VERA_data0

    ldy #2
    lda (sp),y
    sta VERA_data0
    iny
    lda (sp),y
    sta VERA_data0
    ; ---------------------------------
    ldy #1
    lda (sp),y
    sta VERA_data0

    lda (sp)
    asl
    asl
    asl
    asl
    sta ZP_PTR_1
    pla
    ora ZP_PTR_1
    sta VERA_data0


    lda sp
    clc
    adc #10
    sta sp

    rts

; _offset_spr_position(char attr_num, int x_pos, int y_pos)
__offset_spr_position:
    phx
    pha

    lda #1
    sta VERA_ctrl

    ldy #2
    lda (sp), y
    asl
    asl
    asl
    clc
    adc #<(VRAM_sprattr+2)
    sta VERA_addr_low
    stz VERA_ctrl
    sta VERA_addr_low
    
    lda #1
    sta VERA_ctrl
    lda #0
    adc #>(VRAM_sprattr+2)
    sta VERA_addr_high
    stz VERA_ctrl
    sta VERA_addr_high

    lda #1
    sta VERA_ctrl
    lda #%00010001
    sta VERA_addr_bank
    stz VERA_ctrl
    sta VERA_addr_bank

    clc
    lda VERA_data1
    adc (sp)
    inc sp
    sta VERA_data0
    
    lda VERA_data1
    adc (sp)
    inc sp
    sta VERA_data0

    clc
    pla
    adc VERA_data1 
    sta VERA_data0
    
    pla
    adc VERA_data1
    sta VERA_data0

    inc sp
    rts    
; _clear_sprite_attribute(char attr_num)
__clear_sprite_attribute:
    stz VERA_ctrl
    clc
    asl
    asl
    asl
    adc #<VRAM_sprattr + 6
    sta VERA_addr_low
    
    lda #0
    adc #>(VRAM_sprattr)
    sta VERA_addr_high

    lda #%00000001
    sta VERA_addr_bank

    lda VERA_data0
    and #%00001100
    stz VERA_data0

    rts

; _set_layer0_enable(char enable)
; enable - 0 for diable, 1 for enable
__set_layer0_enable:
    bne @enable
    lda VERA_dc_video
    and #%11101111
    sta VERA_dc_video
    rts

    @enable:
    lda VERA_dc_video
    ora #%00010000
    sta VERA_dc_video
    rts

__set_layer1_enable:
    bne @enable
    lda VERA_dc_video
    and #%11011111
    sta VERA_dc_video
    rts

    @enable:
    lda VERA_dc_video
    ora #%00100000
    sta VERA_dc_video
    rts

__set_sprites_enable:
    bne @enable
    lda VERA_dc_video
    and #%10111111
    sta VERA_dc_video
    rts

    @enable:
    lda VERA_dc_video
    ora #%01000000
    sta VERA_dc_video
    rts

CURR_RAM_POS = ZP_PTR_1
INIT_OFFSET = ZP_PTR_3
VRAM_ADDR = ZP_PTR_5
Y_ZP = ZP_PTR_6

; _load_vert_map_sect(char start_pos, char num_of_tiles, char scroll, int pos, int ram_addr, long vram_addr)
__load_vert_map_sect:
    ; +32

    ; saving vram addr
    pha
    phx
    lda sreg
    pha

    ; setting start addr for ram
    ldy #3
    lda (sp), y
    sta CURR_RAM_POS+1

    ldy #2
    lda (sp), y
    sta CURR_RAM_POS
    
    ldx #6
    clc
    @multiply_position:
    lda CURR_RAM_POS
    asl
    sta CURR_RAM_POS
    lda CURR_RAM_POS+1
    rol
    sta CURR_RAM_POS+1
    dex
    bne @multiply_position

    clc
    ldy #0
    lda (sp), y
    adc CURR_RAM_POS
    sta CURR_RAM_POS

    ldy #1
    lda (sp), y
    adc CURR_RAM_POS+1
    sta CURR_RAM_POS+1

    
    pla
    sta VRAM_ADDR+2
    pla
    sta VRAM_ADDR+1
    pla
    sta VRAM_ADDR
    
    ;TODO: edit to handle more than 256 position changes
    clc
    ldy #2
    lda (sp), y
    asl
    and #(%00111111)
    clc
    adc VRAM_ADDR
    sta VRAM_ADDR



    lda VRAM_ADDR+1
    adc #0
    sta VRAM_ADDR+1
    php

    ; adding the scroll offset if the variable was more than 0
    ldy #4
    lda (sp), y
    beq @skip_scroll_off
    lda VRAM_ADDR+1
    clc
    adc #$8
    sta VRAM_ADDR+1

    @skip_scroll_off:
    plp
    lda VRAM_ADDR+2
    adc #0
    sta VRAM_ADDR+2


    ;get start addr for vram
    stz VERA_ctrl
    
    lda VRAM_ADDR
    sta VERA_addr_low

    lda VRAM_ADDR+1
    sta VERA_addr_high

    lda VRAM_ADDR+2
    and #(%00000001)
    ora #(%01110000)
    sta VERA_addr_bank


    lda #1
    sta VERA_ctrl

    clc
    lda VRAM_ADDR
    inc
    sta VERA_addr_low

    lda VRAM_ADDR+1
    adc #0
    sta VERA_addr_high

    lda VRAM_ADDR+2
    adc #0
    ora #(%01110000)
    sta VERA_addr_bank


    ldy #5
    lda (sp),y 
    clc
    ldy #6
    adc (sp),y
    sta Y_ZP

    ldy #6
    lda (sp),y
    beq @skip_loop
    @pad_vram_addr:
    dec
    dec

    ldy VERA_data0
    ldy VERA_data1
    
    cmp #00
    bne @pad_vram_addr

    @skip_loop:
        

    ldy #6
    lda (sp),y
    tay
    @transfer_tile_to_vram:
    lda (CURR_RAM_POS), y
    sta VERA_data0

    iny
    lda (CURR_RAM_POS), y
    sta VERA_data1
    
    iny
    
    cpy Y_ZP
    bne @transfer_tile_to_vram


    clc 
    lda sp
    adc #7
    sta sp

    rts

; _check_collision_data(int col_data_addr, int x, int y)
; PS: the x and y values of object positions are made 
TILE_ADDR = ZP_PTR_1
__check_collision_data:
    ; getting tile addr based on y pos
    lsr
    lsr
    lsr
    lsr
    sta TILE_ADDR

    ldy #1
    lda (sp),y
    pha

    ; getting tile addr based on x pos
    lda (sp)
    inc sp
    inc sp
    and #%11110000

    ora TILE_ADDR

    ; add base address for collistion data
    clc
    adc (sp)
    inc sp
    sta TILE_ADDR
    
    lda (sp)
    inc sp
    adc #0
    sta TILE_ADDR+1
    pla

    clc
    adc TILE_ADDR+1
    sta TILE_ADDR+1

    ; get and return collision data
    lda (TILE_ADDR)
    rts

; _transfer_ram_to_vram(int ram_src_addr, int size, char vram_bank, int vram_des_addr)
RAM_ADDR = ZP_PTR_1
__transfer_ram_to_vram:
    ;set vram data port
    stz VERA_ctrl
    sta VERA_addr_low
    stx VERA_addr_high

    lda (sp)
    inc sp
    ora #%00010000
    sta VERA_addr_bank
    

    ; set up x and y with the size of transfer for loop
    ldy #1
    lda (sp),y
    tax

    lda (sp)
    tay

    inc sp
    inc sp

    ; saving ram_addr
    lda (sp)
    inc sp
    sta RAM_ADDR

    lda (sp)
    inc sp
    sta RAM_ADDR+1

    ;looping over and transfer
    @transfer_data:
    lda (RAM_ADDR)
    sta VERA_data0

    inc RAM_ADDR

    bne @leave_to_high_byte_unchanged
    inc RAM_ADDR+1
    @leave_to_high_byte_unchanged:

    dey
    cpy #255
    bne @transfer_data

    dex
    cpx #255
    beq @end_loop
    bne @transfer_data

    @end_loop:
    rts

; _transfer_ram_to_vram(char pal_off, int spr_addr_offset, int x_pos, int y_pos, int ram_src_addr, char vram_bank, int vram_des_addr)
SPR_OFF_VAR = ZP_PTR_3
__transfer_spr_attr_to_vram:
    ;set vram data port
    stz VERA_ctrl
    sta VERA_addr_low
    stx VERA_addr_high

    lda (sp)
    inc sp
    ora #%00010000
    sta VERA_addr_bank

    ; saving ram_addr
    lda (sp)
    inc sp
    sta RAM_ADDR

    lda (sp)
    inc sp
    sta RAM_ADDR+1



    ;looping over and transfer
    ldx #0
    @transfer_data:
    lda (RAM_ADDR)

    cpx #0
    beq @spr_off_low
    cpx #1
    beq @spr_off_high
    cpx #2
    beq @x_low
    cpx #3
    beq @x_high
    cpx #4
    beq @y_low
    cpx #5
    beq @y_high

    cpx #7
    beq @pal_offset

    bra @continue_loop

    @pal_offset:
    ldy #6
    ora (sp),y

    bra @continue_loop

    @spr_off_low:
    pha ; push the original sprite address to the stack

    ldy #4
    lda (sp),y
    lsr
    sta SPR_OFF_VAR

    ldy #5
    lda (sp),y
    asl
    asl
    asl
    asl
    asl
    asl
    asl
    ora SPR_OFF_VAR
    sta SPR_OFF_VAR

    pla 
    clc
    adc SPR_OFF_VAR
    php
    
    bra @continue_loop

    @spr_off_high:
    pha ; push the original sprite address to the stack

    ldy #5
    lda (sp),y
    lsr 
    sta SPR_OFF_VAR
    pla
    plp
    adc SPR_OFF_VAR

    bra @continue_loop

    @x_low:
    clc
    ldy #2
    adc (sp),y
    php
    bra @continue_loop

    @x_high:
    plp
    ldy #3
    adc (sp),y
    bra @continue_loop

    @y_low:
    clc
    ldy #0
    adc (sp),y
    php
    bra @continue_loop
    
    @y_high:
    plp
    ldy #1
    adc (sp),y
    bra @continue_loop

    @continue_loop:
    sta VERA_data0
    inc RAM_ADDR

    bne @leave_to_high_byte_unchanged
    inc RAM_ADDR+1
    @leave_to_high_byte_unchanged:

    inx
    cpx #8
    bne @transfer_data

    inc sp
    inc sp
    inc sp
    inc sp

    inc sp
    inc sp
    inc sp
    rts


VSYNC_FLAG = $70
__wait_for_nmi:
    lda VSYNC_FLAG
    beq __wait_for_nmi

    stz VSYNC_FLAG
    rts

default_irq: .addr 0
__init_irq_handler:
    lda IRQVec
    sta default_irq
    lda IRQVec+1
    sta default_irq+1

    sei

    lda #<custom_irq
    sta IRQVec
    lda #>custom_irq
    sta IRQVec+1

    lda VERA_ien
    ora #%10000011

    lda #224
    sta VERA_irqline_l

    cli
    rts

custom_irq:
    lda VERA_isr
    bit #%00000001
    beq @run_default_irq

    lda #1
    sta VSYNC_FLAG

    @run_default_irq:
    jmp (default_irq)


joystick_get = $FF56
__get_joystick_state:
    lda #0
    jsr joystick_get

    pha
    txa
    eor #$FF
    tax

    pla
    eor #$FF

    rts

__force_halt:
    brk
    rts