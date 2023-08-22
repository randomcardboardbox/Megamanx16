.include "x16.inc"
.include "zeropage.inc"

.export __m_apply_velocity

; _m_apply_velocity(int obj_addr)
OBJ_ADDR = ZP_PTR_1
x_vel = ZP_PTR_3
x_vel_frac = ZP_PTR_4

y_vel = ZP_PTR_5
y_vel_frac = ZP_PTR_6

__m_apply_velocity:
    ; 0 - x - low
    ; 1 - x - high
    ; 2 - x_frac
    ; 3 - y - low
    ; 4 - y - high
    ; 5 - y_frac

    ; 6 - x_vel
    ; 7 - x_vel_frac
    ; 8 - y_vel
    ; 9 - y_vel_frac

    sta OBJ_ADDR
    stx OBJ_ADDR+1

    ldy #6
    lda (OBJ_ADDR), y 
    sta x_vel
    ldy #7
    lda (OBJ_ADDR), y 
    sta x_vel_frac

    bbs7 x_vel, @sub_x_vel
    lda x_vel
    cmp #0
    bne @add_vel

    bbs7 x_vel_frac, @sub_x_vel_frac

    @add_vel:
    ;adding x velocity

    ldy #7
    lda (OBJ_ADDR), y

    ldy #2
    clc
    adc (OBJ_ADDR), y
    sta (OBJ_ADDR), y

    ldy #6
    lda (OBJ_ADDR), y

    ldy #0
    adc (OBJ_ADDR), y
    sta (OBJ_ADDR), y

    ldy #1
    lda (OBJ_ADDR), y
    adc #0
    sta (OBJ_ADDR), y
    
    bra @add_y_vel

    @sub_x_vel_frac:
    lda #$FF
    sec
    sbc x_vel_frac
    sta x_vel_frac

    ldy #2
    lda (OBJ_ADDR), y

    sec
    sbc x_vel_frac
    sta (OBJ_ADDR), y
    

    ldy #0
    lda (OBJ_ADDR), y

    sbc #0
    sta (OBJ_ADDR), y


    ldy #1
    lda (OBJ_ADDR), y
    sbc #0
    sta (OBJ_ADDR), y

    bra @add_y_vel

    @sub_x_vel:
    lda #$FF
    sec
    sbc x_vel_frac
    sta x_vel_frac

    ldy #2
    lda (OBJ_ADDR), y

    sec
    sbc x_vel_frac
    php
    sta (OBJ_ADDR), y

    lda #$FF
    sec
    sbc x_vel
    sta x_vel

    ldy #0
    lda (OBJ_ADDR), y

    plp
    sbc x_vel
    sta (OBJ_ADDR), y

    ldy #1
    lda (OBJ_ADDR), y
    sbc #0
    sta (OBJ_ADDR), y

    @add_y_vel:
    ;adding y velocity
    ; TODO: add subtraction routine for y velocity

    ldy #8
    lda (OBJ_ADDR), y 
    sta y_vel
    ldy #9
    lda (OBJ_ADDR), y 
    sta y_vel_frac

    bbs7 y_vel, @sub_y_vel
    lda y_vel
    cmp #0
    bne @add_vely

    bbs7 y_vel_frac, @sub_y_vel_frac
    ;adding y velocity

    @add_vely:

    ldy #9
    lda (OBJ_ADDR), y

    ldy #5
    clc
    adc (OBJ_ADDR), y
    sta (OBJ_ADDR), y

    ldy #8
    lda (OBJ_ADDR), y

    ldy #3
    adc (OBJ_ADDR), y
    sta (OBJ_ADDR), y

    ldy #4
    lda (OBJ_ADDR), y
    adc #0
    sta (OBJ_ADDR), y
    
    bra @end_sub

    @sub_y_vel_frac:
    lda #$FF
    sec
    sbc y_vel_frac
    sta y_vel_frac

    ldy #5
    lda (OBJ_ADDR), y

    sec
    sbc y_vel_frac
    sta (OBJ_ADDR), y
    

    ldy #3
    lda (OBJ_ADDR), y

    sbc #0
    sta (OBJ_ADDR), y


    ldy #4
    lda (OBJ_ADDR), y
    sbc #0
    sta (OBJ_ADDR), y

    bra @end_sub

    @sub_y_vel:
    lda #$FF
    sec
    sbc y_vel_frac
    sta y_vel_frac

    ldy #5
    lda (OBJ_ADDR), y

    sec
    sbc y_vel_frac
    php

    lda #$FF
    sec
    sbc y_vel
    sta y_vel

    ldy #3
    lda (OBJ_ADDR), y

    plp
    sbc y_vel
    sta (OBJ_ADDR), y

    ldy #4
    lda (OBJ_ADDR), y
    sbc #0
    sta (OBJ_ADDR), y

    @end_sub:
    rts


OBJ_X = 0
FRAC_X = 2
OBJ_Y = 3
FRAC_Y = 5

X_VEL = 6
X_FRAC_VEL = 7
Y_VEL = 8
Y_FRAC_VEL = 9

HEALTH = 10
STATUS = 11


ANIM_TIMER = 12
FRAME = 14
ANIM_INDEX = 15
ANIM_ADDR = 16
NO_OF_SPRS = 18
SPR_IND = 19

; ; _play_obj_anim_frame(int base_addr, int obj, char no_of_sprs, int VRAM_sprattr)
; __play_anim_frame:
;     ; set up vram data port
;     stz VERA_ctrl

;     sta VERA_addr_low
;     stx VERA_addr_high

;     lda #%00010001
;     sta VERA_addr_bank

;     ; storing arguments into zero page
;     lda (sp)
;     sta NO_OF_SPRS
;     inc sp

;     lda (sp)
;     sta OBJECT_ADDR
;     inc sp
;     lda (sp)
;     sta OBJECT_ADDR+1
;     inc sp

;     lda (sp)
;     sta BASE_ADDR
;     inc sp
;     lda (sp)
;     sta BASE_ADDR+1
;     inc sp


;     ; store sprite offset data
;     lda (sp)
;     lsr
;     sta SPR_OFF_VAR
;     inc sp

;     lda (sp)
;     pha
;     lsr
;     sta SPR_OFF_VAR+1

;     pla
;     asl
;     asl
;     asl
;     asl
;     asl
;     asl
;     asl
;     ora SPR_OFF_VAR
;     sta SPR_OFF_VAR
;     inc sp


;     lda (sp)
;     sta PAL_OFF
;     inc sp


;     ; setting x coords
    

;     ldx NO_OF_SPRS
;     @loop_over_sprs:

;     ldy #0
;     @loop_over_attrs:
;     lda (BASE_ADDR)


;     cpy #0
;     beq @spr_off_low
;     cpy #1
;     beq @spr_off_high
;     cpy #7
;     beq @pal_offset

;     bra @continue_loop

;     @pal_offset:
;     ora PAL_OFF
;     bra @continue_loop
    
;     @spr_off_low:
;     clc 
;     adc SPR_OFF_VAR
;     php
;     bra @continue_loop

;     @spr_off_high:
;     plp
;     adc SPR_OFF_VAR+1
;     bra @continue_loop


;     @continue_loop:
;     sta VERA_data0
    
;     inc BASE_ADDR
;     bne @leave_to_high_byte_unchanged
;     inc BASE_ADDR+1
;     @leave_to_high_byte_unchanged:

    
;     iny
;     cpy #8
;     bne @loop_over_attrs


;     dex
;     cpx #0
;     bne @loop_over_sprs

;     rts