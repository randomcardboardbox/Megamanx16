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
    
    ; sta $0040
    ; brk

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