.include "x16.inc"
.include "zeropage.inc"

.export __update_objects

NO_OF_OBJs = 50
OBJ_BYTE_SIZE = 28
OBJ_DEF_BYTE_SIZE = 14
; _update_objects(int obj_arr_addr, int obj_def_addr)
OBJ_ADDR = ZP_PTR_1
__update_objects:
    ldx #0
    @loop_over_objs:


    inx
    cpx #NO_OF_OBJs
    bne @loop_over_objs

    inc sp
    inc sp

    rts
