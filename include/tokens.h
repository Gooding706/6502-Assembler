#pragma once
#ifndef TOKEN
#define TOKEN

#include <stdbool.h>
#include <errors.h>

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

#define isAccumulatorAddressable(instruction) isTokenOfType(instruction, 4, ASL, LSR, ROL, ROR)
#define isAbsoluteAddressable(instruction) isTokenOfType(instruction, 23, ADC, AND, ASL, BIT, CMP, CPX, CPY, DEC, EOR, INC, JMP, JSR, LDA, LDX, LDY, LSR, ORA, ROL, ROR, SBC, STA, STX, STY)
#define isAbsoluteXAddressable(instruction) isTokenOfType(instruction, 15, ADC, AND, ASL, CMP, DEC, EOR, INC, LDA, LDY, LSR, ORA, ROL, ROR, SBC, STA)
#define isAbsoluteYAddressable(instruction) isTokenOfType(instruction, 9, ADC, AND, CMP, EOR, LDA, LDX, ORA, SBC, STA)
#define isImmediateAddressable(instruction) isTokenOfType(instruction, 11, ADC, AND, CMP, CPX, CPY, EOR, LDX, LDA, LDY, ORA, SBC)
#define isImpliedAddressable(instruction) isTokenOfType(instruction, 25, BRK, CLC, CLD, CLI, CLV, DEX, DEY, INX, INY, NOP, PHA, PHP, PLA, PLP, RTI, RTS, SEC, SED, SEI, TAX, TAY, TSX, TXA, TXS, TYA)
#define isIndirectAddressable(instruction) isTokenOfType(instruction, 1, JMP)
#define isIndirectXAddressable(instruction) isTokenOfType(instruction, 8, ADC, AND, CMP, EOR, LDA, ORA, SBC, STA)
#define isIndirectYAddressable(instruction) isTokenOfType(instruction, 8, ADC, AND, CMP, EOR, LDA, ORA, SBC, STA)
#define isRelativeAddressable(instruction) isTokenOfType(instruction, 8, BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS)
#define isZeroPageAddressable(instruction) isTokenOfType(instruction, 21, ADC, AND, ASL,BIT, CMP, CPX, CPY, DEC, EOR, INC, LDA, LDX, LDY, LSR, ORA, ROL, ROR, SBC, STA, STX, STY)
#define isZeroPageXAddressable(instruction) isTokenOfType(instruction, 16, ADC, AND, ASL, CMP, DEC, EOR, INC, LDA, LDY, LSR, ORA, ROL, ROR, SBC, STA, STY)
#define isZeroPageYAddressable(instruction) isTokenOfType(instruction, 2, LDX, STX)

typedef struct
{
    unsigned short tokenId;
    char *textContent;
    errorData* err;
} token;

typedef struct
{
    token *content;
    int capacity;
    int length;
} tokenList;

void pushToken(token t, tokenList *list);
void freeTokenList(tokenList *list);

bool isTokenOfType(int id, int count, ...);

#endif
