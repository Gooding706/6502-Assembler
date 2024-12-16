.define apple           #$a4 ;dsasa
.define orange           $a4ff ;casa

.define appleL         $00 ; screen location of apple, low byte
.define appleH         $01 ; screen location of apple, high byte
.define snakeHeadL     $10 ; screen location of snake head, low byte
.define snakeHeadH     $11 ; screen location of snake head, high byte

.define up 5

LDA appleL
LDX appleH
STA orange,x

ORA #up
ADC #down


.define down 45