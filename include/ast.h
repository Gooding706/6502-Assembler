#pragma once
#ifndef AST
#define AST

#include <stdlib.h>

// an adress can either be a 16 bit value or an unresolved label
typedef struct
{
    enum
    {
        unresolvedLabel,
        constant
    } tag;

    union
    {
        uint16_t addressLiteral;
        char *labelText;
    }data;
} address_t;

// dynamic arrays
typedef struct
{
    address_t *content;
    int capacity;
    int length;
} u16List;

typedef struct
{
    uint8_t *content;
    int capacity;
    int length;
} u8List;
//

typedef struct
{
    unsigned short opcode;
    address_t address;
} absoluteInstruction;

typedef struct
{
    unsigned short opcode;
    uint8_t value;
} immediateInstruction;

typedef absoluteInstruction indirectInstruction;
typedef absoluteInstruction relativeInstruction;

typedef immediateInstruction indirectRegisterInstruction;
typedef immediateInstruction zeroPageInstruction;

/*
It turns out that 6502 assembly is not quite complicated enough to really warrant
a proper AST rather we hold a list of tagged unions. We encode the adressing mode
of the instruction which will make generating bytecode easier.
*/
typedef struct
{
    enum tag
    {
        label,
        directiveBytes,
        directiveWords,
        instruction,
        // Different addressing modes
        accumulator,
        absolute,
        absoluteX,
        absoluteY,
        immediate,
        implied,
        indirect,
        indirectX,
        indirectY,
        relative,
        zeroPage,
        zeroPageX,
        zeroPageY,
    } branchType;

    union
    {
        char *labelText;
        u16List wordDirective;
        u8List byteDirective;

        unsigned short opcodeOnly;        // accumulator / implied instructions
        absoluteInstruction absoluteMode; // absolute, absoluteX, absoluteY
        immediateInstruction immediateMode;
        indirectInstruction indirectMode;                 // indirect
        indirectRegisterInstruction indirectRegisterMode; // indirectX, indirectY
        relativeInstruction relativeMode;
        zeroPageInstruction zeroPageMode; // zeroPage, zeroPageX, ZeroPageY
    } data;

} astBranch;

typedef struct
{
    astBranch *content;
    int capacity;
    int length;
} ast;

void pushU16(uint16_t u16, u16List *list);
void pushU8(uint8_t u8, u8List *list);
void pushBranch(astBranch branch, ast *list);

#endif