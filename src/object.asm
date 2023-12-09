.include "x16.inc"
.include "zeropage.inc"

.export __update_objects
.export __draw_objects
.export __play_obj_anim_frame
.export __update_obj_pos


; _update_objects(int obj_arr_addr, int obj_def_addr, int scrollx, int func_addr)

NO_OF_OBJs = 20
OBJ_BYTE_SIZE = 32
OBJ_DEF_BYTE_SIZE = 14

OBJ_ADDR = ZP_PTR_1
ATTR_ADDR = ZP_PTR_3
JMP_ADDR = ZP_PTR_4
SCROLL_X = ZP_PTR_5
DEALLOC_FUNC = ZP_PTR_8


OBJ_X = 0
FRAC_X = 2
OBJ_Y = 3
FRAC_Y = 5

X_VEL = 6
X_FRAC_VEL = 7
Y_VEL = 8
Y_FRAC_VEL = 9

OBJ_TYPE_REF = 10
SPAWN_ID = 11

HEALTH = 12
STATUS = 13

ANIM_TIMER = 14
FRAME = 16
ANIM_INDEX = 17
SPR_IND = 18

TIMER1 = 19
TIMER2 = 21
TIMER3 = 23

VAR1 = 25
VAR2 = 26
VAR3 = 27

UPDATE_FUNC = 28
DRAW_FUNC = 30

__update_objects:

    ; store the object definition array address in the zero page
    sta DEALLOC_FUNC
    stx DEALLOC_FUNC+1


    lda (sp),y
    sta SCROLL_X
    inc sp
    lda (sp),y
    sta SCROLL_X+1
    inc sp

    ; store the object array address in the zero page
    lda (sp),y
    sta OBJ_ADDR
    inc sp
    lda (sp),y
    sta OBJ_ADDR+1
    inc sp

    ; loop over each object
    ldx #0
    @loop_over_objs:

    ; check if a valid object is in this position in the array
    ldy #OBJ_TYPE_REF
    lda (OBJ_ADDR), y
    cmp #0
    beq @continue_loop

    ;run object update function
    ldy #UPDATE_FUNC
    lda (OBJ_ADDR), y
    sta JMP_ADDR
    iny
    lda (OBJ_ADDR), y
    sta JMP_ADDR+1

    phx
    phy
    txa
    jsr __run_func_from_ptr
    ply
    plx

    ; decrement object timers
    phx
    ldx #3
    ldy #TIMER1

    ; decrement object timers
    @dec_timers:
    lda (OBJ_ADDR), y
    sec
    sbc #1
    sta (OBJ_ADDR), y

    iny
    lda (OBJ_ADDR), y
    sbc #0
    iny
    bcs @set_timer_val

    lda #0
    dey
    sta (OBJ_ADDR), y
    dey
    sta (OBJ_ADDR), y
    iny
    iny

    bra @loop_timers

    @set_timer_val:
    sta (OBJ_ADDR), y

    @loop_timers:
    dex 
    cpx #0
    bne @dec_timers
    plx


    ;check if object needs to be despawned;
    ldy #OBJ_X
    lda (OBJ_ADDR),y
    asl 
    iny
    lda (OBJ_ADDR),y
    rol

    sec
    sbc SCROLL_X+1

    cmp #$3
    bpl @dealloc
    cmp #$FF
    beq @dealloc

    bra @continue_loop

    @dealloc:
    phx
    txa
    jsr __dealloc_object
    plx

    @continue_loop:
    ; increment the object address to access the next object in the list
    lda OBJ_ADDR
    clc
    adc #OBJ_BYTE_SIZE
    sta OBJ_ADDR

    lda OBJ_ADDR+1
    adc #0
    sta OBJ_ADDR+1

    ; loop over objects
    inx
    cpx #NO_OF_OBJs
    bne @loop_over_objs

    rts

__run_func_from_ptr:
    jmp (JMP_ADDR)

__dealloc_object:
    jmp (DEALLOC_FUNC)
    
__run_draw_func:
    jmp (JMP_ADDR2)

; _play_obj_anim_frame(int x, int y, char pal_off, int base_addr, char no_of_sprs, int VRAM_sprattr)
BASE_ADDR = ZP_PTR_1
OBJECT_ADDR = ZP_PTR_3
NO_OF_SPRS = ZP_PTR_5

SPR_OFF_VAR = ZP_PTR_4
PAL_OFF = ZP_PTR_6

X_OFF = ZP_PTR_10
Y_OFF = ZP_PTR_11

__play_obj_anim_frame:
    ; set up vram data port
    stz VERA_ctrl

    sta VERA_addr_low
    stx VERA_addr_high

    lda #%00010001
    sta VERA_addr_bank

    ; storing arguments into zero page
    lda (sp)
    sta NO_OF_SPRS
    inc sp

    lda (sp)
    sta BASE_ADDR
    inc sp
    lda (sp)
    sta BASE_ADDR+1
    inc sp


    ; store sprite offset data
    lda (sp)
    lsr
    sta SPR_OFF_VAR
    inc sp

    lda (sp)
    pha
    lsr
    sta SPR_OFF_VAR+1

    pla
    asl
    asl
    asl
    asl
    asl
    asl
    asl
    ora SPR_OFF_VAR
    sta SPR_OFF_VAR
    inc sp


    lda (sp)
    sta PAL_OFF
    inc sp

    lda (sp)
    sta Y_OFF
    inc sp
    lda (sp)
    sta Y_OFF+1
    inc sp

    lda (sp)
    sta X_OFF
    inc sp
    lda (sp)
    sta X_OFF+1
    inc sp


    ; setting x coords
    

    ldx NO_OF_SPRS
    @loop_over_sprs:

    ldy #0
    @loop_over_attrs:
    lda (BASE_ADDR)


    cpy #0
    beq @spr_off_low
    cpy #1
    beq @spr_off_high
    cpy #7
    beq @pal_offset

    cpy #2
    beq @x_low
    cpy #3
    beq @x_high
    cpy #4
    beq @y_low
    cpy #5
    beq @y_high


    bra @continue_loop

    @pal_offset:
    ora PAL_OFF
    bra @continue_loop
    
    @spr_off_low:
    clc 
    adc SPR_OFF_VAR
    php
    bra @continue_loop

    @spr_off_high:
    plp
    adc SPR_OFF_VAR+1
    bra @continue_loop

    @x_low:
    clc
    adc X_OFF
    php
    bra @continue_loop

    @x_high:
    plp
    adc X_OFF+1
    bra @continue_loop

    @y_low:
    clc
    adc Y_OFF
    php
    bra @continue_loop
    
    @y_high:
    plp
    adc Y_OFF+1
    bra @continue_loop


    @continue_loop:
    sta VERA_data0
    
    inc BASE_ADDR
    bne @leave_to_high_byte_unchanged
    inc BASE_ADDR+1
    @leave_to_high_byte_unchanged:

    
    iny
    cpy #8
    bne @loop_over_attrs


    dex
    cpx #0
    bne @loop_over_sprs

    rts

JMP_ADDR2 = ZP_PTR_9
OBJ_ADDR2 = ZP_PTR_8
__draw_objects:
    sta OBJ_ADDR2
    stx OBJ_ADDR2+1

    lda (sp)
    sta JMP_ADDR2
    inc sp
    lda (sp)
    sta JMP_ADDR2+1
    inc sp

    ldx #0
    @loop_over_objs:

    ; check if a valid object is in this position in the array
    ldy #OBJ_TYPE_REF
    lda (OBJ_ADDR2), y
    cmp #0
    beq @continue_loop

    ; run draw function  
    phx
    txa
    jsr __run_draw_func
    plx


    @continue_loop:
    ; increment the object address to access the next object in the list
    lda OBJ_ADDR2
    clc
    adc #OBJ_BYTE_SIZE
    sta OBJ_ADDR2

    lda OBJ_ADDR2+1
    adc #0
    sta OBJ_ADDR2+1

    ; loop over objects
    inx
    cpx #NO_OF_OBJs
    bne @loop_over_objs

    rts



;_update_obj_pos(int x, int y, char pal_off, int base_addr, char no_of_sprs, int VRAM_sprattr)
__update_obj_pos:
 rts