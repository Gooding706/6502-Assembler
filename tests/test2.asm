bytes:
    .byte   $ab, $bc, $ff, %01101101, 124
    .byte   $ab, $fe, $ff, %01101101
    .word   123, 1560, $ffaa, gamer

main:
    BNE     bytes
    LDA     bytes
    LDA     bytes,  X
    LDA     bytes,  Y
    LDA     #$ff
    NOP
    ROL
    STA     $00
    STA     $00,    X
    STX     $00,    Y
    JMP ($9000)
    LDA ($05,x)
    LDA ($10),y 
