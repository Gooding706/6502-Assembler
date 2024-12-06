#pragma once
#ifndef TOKEN
#define TOKEN

#define ADC 0
#define AND 1
#define ASL 2
#define BCC 3
#define BCS 4
#define BEQ 5
#define BIT 6
#define BMI 7
#define BNE 8
#define BPL 9
#define BRK 10
#define BVC 11
#define BVS 12
#define CLC 13
#define CLD 14
#define CLI 15
#define CLV 16
#define CMP 17
#define CPX 18
#define CPY 19
#define DEC 20
#define DEX 21
#define DEY 22
#define EOR 23
#define INC 24
#define INX 25
#define INY 26
#define JMP 27
#define JSR 28
#define LDA 29
#define LDX 30
#define LDY 31
#define LSR 32
#define NOP 33
#define ORA 34
#define PHA 35
#define PHP 36
#define PLA 37
#define PLP 38
#define ROL 39
#define ROR 40
#define RTI 41
#define RTS 42
#define SBC 43
#define SEC 44
#define SED 45
#define SEI 46
#define STA 47
#define STX 48
#define STY 49
#define TAX 50
#define TAY 51
#define TSX 52
#define TXA 53
#define TXS 54
#define TYA 55

#define HASH 56
#define DOLLAR 57
#define COLON 58
#define PERCENT 59
#define COMMA 60
#define NEWLINE 61
#define OPENPAREN 62
#define CLOSEPAREN 63
#define FILEEND 64
#define PERIOD 65

#define LABEL 66
#define BYTEDIRECTIVE 67
#define WORDDIRECTIVE 68
#define XREGISTER 69
#define YREGISTER 70

#define HEXNUMBER_8 71
#define HEXNUMBER_16 72
#define BINARYNUMBER_8 73
#define BINARYNUMBER_16 74
#define NUMBER_8 75
#define NUMBER_16 76

typedef struct
{
    unsigned short tokenId;
    char* textContent;
} token;

typedef struct
{
    token* content;
    int capacity;
    int length;
} tokenList;

void pushToken(token t, tokenList* list);
void freeTokenList(tokenList* list);


#endif
