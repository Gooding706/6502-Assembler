#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include <ast.h>
#include <parser.h>
#include <errors.h>

typedef struct
{
    char *labelText;
    uint16_t addressOffset;
} resolvedLabel;

typedef struct
{
    resolvedLabel *content;
    int capacity;
    int length;
} labelList;

char *takeLabelBranchText(astBranch *branch)
{
    char *out = branch->data.labelText;
    branch->data.labelText = NULL;
    return out;
}

void pushLabel(char *label, uint16_t offset, labelList *list)
{
    if (list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(resolvedLabel));
    }

    list->content[list->length] = (resolvedLabel){label, offset};
    list->length++;
}

void freeLabelList(labelList *list)
{
    for (int i = 0; i < list->length; i++)
    {
        free(list->content[i].labelText);
    }
    free(list->content);
}

int branchSize(astBranch *branch)
{
    switch (branch->branchType)
    {
    case directiveBytes:
        return branch->data.byteDirective.length;
    case directiveWords:
        return branch->data.wordDirective.length * 2;
    case accumulator:
        return 1;
    case absolute:
        return 3;
    case absoluteX:
        return 3;
    case absoluteY:
        return 3;
    case immediate:
        return 2;
    case implied:
        return 1;
    case indirect:
        return 3;
    case indirectX:
        return 2;
    case indirectY:
        return 2;
    case relative:
        return 2;
    case zeroPage:
        return 2;
    case zeroPageX:
        return 2;
    case zeroPageY:
        return 2;
    default:
        return 0;
    }
}
// fill labels list and return length (in bytes) of the entire program
int fillLabelList(ast *list, labelList *labels)
{
    int fileLength = 0;
    for (int i = 0; i < list->length; i++)
    {
        if (list->content[i].branchType == label)
        {
            pushLabel(takeLabelBranchText(&list->content[i]), fileLength, labels);
            continue;
        }

        fileLength += branchSize(&list->content[i]);
    }

    return fileLength;
}

bool matchLabel(char *labelText, labelList *list, int programOffset, uint16_t *value)
{
    for (int i = 0; i < list->length; i++)
    {
        if (!strcmp(labelText, list->content[i].labelText))
        {
            *value = list->content[i].addressOffset + programOffset;
            return true;
        }
    }

    return false;
}

int resolveAddress(address_t addr, labelList *list, int programOffset, uint16_t *out)
{
    if (addr.tag == unresolvedLabel)
    {
        if (!matchLabel(addr.data.labelText, list, programOffset, out))
        {
            return UNKNOWNLABEL;
        }
    }
    else
    {
        *out = addr.data.addressLiteral;
    }

    return SUCCESS;
}

// TODO: convert endianess for big endian systems
int resolveWordList(astBranch *branch, int programOffset, labelList *labelTable, char **outputData)
{
    for (int i = 0; i < branch->data.wordDirective.length; i++)
    {
        address_t currentWord = branch->data.wordDirective.content[i];
        if (currentWord.tag == label)
        {
            if (!matchLabel(currentWord.data.labelText, labelTable, programOffset, (uint16_t *)(*outputData)))
            {
                return UNKNOWNLABEL;
            }
        }
        else
        {
            *((uint16_t *)(*outputData)) = branch->data.wordDirective.content[i].data.addressLiteral;
        }
        *outputData += 2;
    }
    return SUCCESS;
}

int resolveImpliedInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.opcodeOnly)
    {
    case BRK:
        code = 0x00;
        break;
    case CLC:
        code = 0x18;
        break;
    case CLD:
        code = 0xD8;
        break;
    case CLI:
        code = 0x58;
        break;
    case CLV:
        code = 0xB8;
        break;
    case DEX:
        code = 0xCA;
        break;
    case DEY:
        code = 0x88;
        break;
    case INX:
        code = 0xE8;
        break;
    case INY:
        code = 0xC8;
        break;
    case NOP:
        code = 0xEA;
        break;
    case PHA:
        code = 0x48;
        break;
    case PHP:
        code = 0x08;
        break;
    case PLA:
        code = 0x68;
        break;
    case PLP:
        code = 0x28;
        break;
    case RTI:
        code = 0x40;
        break;
    case RTS:
        code = 0x60;
        break;
    case SEC:
        code = 0x38;
        break;
    case SED:
        code = 0xF8;
        break;
    case SEI:
        code = 0x78;
        break;
    case TAX:
        code = 0xAA;
        break;
    case TAY:
        code = 0xA8;
        break;
    case TSX:
        code = 0xBA;
        break;
    case TXA:
        code = 0x8A;
        break;
    case TXS:
        code = 0x9A;
        break;
    case TYA:
        code = 0x98;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    return SUCCESS;
}

int resolveAccumulatorInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.opcodeOnly)
    {
    case ASL:
        code = 0x0A;
        break;
    case LSR:
        code = 0x4A;
        break;
    case ROL:
        code = 0x2A;
        break;
    case ROR:
        code = 0x6A;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    return SUCCESS;
}

int resolveImmediateInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.immediateMode.opcode)
    {
    case ADC:
        code = 0x69;
        break;
    case AND:
        code = 0x29;
        break;
    case CMP:
        code = 0xC9;
        break;
    case CPX:
        code = 0xE0;
        break;
    case CPY:
        code = 0xC0;
        break;
    case EOR:
        code = 0x49;
        break;
    case LDA:
        code = 0xA9;
        break;
    case LDX:
        code = 0xA2;
        break;
    case LDY:
        code = 0xA0;
        break;
    case ORA:
        code = 0x09;
        break;
    case SBC:
        code = 0xE9;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.immediateMode.value;
    *outputData += 1;
    return SUCCESS;
}

int resolveAbsoluteInstruction(astBranch *branch, int programOffset, labelList *labelTable, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.absoluteMode.opcode)
    {
    case ADC:
        code = 0x6D;
        break;
    case AND:
        code = 0x2D;
        break;
    case ASL:
        code = 0x0E;
        break;
    case BIT:
        code = 0x2C;
        break;
    case CMP:
        code = 0xCD;
        break;
    case CPX:
        code = 0xEC;
        break;
    case CPY:
        code = 0xCC;
        break;
    case DEC:
        code = 0xCE;
        break;
    case EOR:
        code = 0x4D;
        break;
    case INC:
        code = 0xEE;
        break;
    case JMP:
        code = 0x4C;
        break;
    case JSR:
        code = 0x20;
        break;
    case LDA:
        code = 0xAD;
        break;
    case LDX:
        code = 0xAE;
        break;
    case LDY:
        code = 0xAC;
        break;
    case LSR:
        code = 0x4E;
        break;
    case ORA:
        code = 0x0D;
        break;
    case ROL:
        code = 0x2E;
        break;
    case ROR:
        code = 0x6E;
        break;
    case SBC:
        code = 0xED;
        break;
    case STA:
        code = 0x8D;
        break;
    case STX:
        code = 0x8E;
        break;
    case STY:
        code = 0x8C;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    int returnVal = resolveAddress(branch->data.absoluteMode.address, labelTable, programOffset, (uint16_t *)*outputData);
    *outputData += 2;
    return returnVal;
}

int resolveAbsoluteXInstruction(astBranch *branch, int programOffset, labelList *labelTable, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.absoluteMode.opcode)
    {
    case ADC:
        code = 0x7D;
        break;
    case AND:
        code = 0x3D;
        break;
    case ASL:
        code = 0x1E;
        break;
    case CMP:
        code = 0xDD;
        break;
    case DEC:
        code = 0xDE;
        break;
    case EOR:
        code = 0x5D;
        break;
    case INC:
        code = 0xFE;
        break;
    case LDA:
        code = 0xBD;
        break;
    case LDY:
        code = 0xBC;
        break;
    case LSR:
        code = 0x5E;
        break;
    case ORA:
        code = 0x1D;
        break;
    case ROL:
        code = 0x3E;
        break;
    case ROR:
        code = 0x7E;
        break;
    case SBC:
        code = 0xFD;
        break;
    case STA:
        code = 0x9D;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    int returnVal = resolveAddress(branch->data.absoluteMode.address, labelTable, programOffset, (uint16_t *)*outputData);
    *outputData += 2;
    return returnVal;
}

int resolveAbsoluteYInstruction(astBranch *branch, int programOffset, labelList *labelTable, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.absoluteMode.opcode)
    {
    case ADC:
        code = 0x79;
        break;
    case AND:
        code = 0x39;
        break;
    case CMP:
        code = 0xD9;
        break;
    case EOR:
        code = 0x59;
        break;
    case LDA:
        code = 0xB9;
        break;
    case LDX:
        code = 0xBE;
        break;
    case ORA:
        code = 0x19;
        break;
    case SBC:
        code = 0xF9;
        break;
    case STA:
        code = 0x99;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    int returnVal = resolveAddress(branch->data.absoluteMode.address, labelTable, programOffset, (uint16_t *)*outputData);
    *outputData += 2;
    return returnVal;
}

int resolveRelativeInstruction(astBranch *branch, int instructionOffset, int programOffset, labelList *labelTable, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.relativeMode.opcode)
    {
    case BCC:
        code = 0x90;
        break;
    case BCS:
        code = 0xB0;
        break;
    case BEQ:
        code = 0xF0;
        break;
    case BMI:
        code = 0x30;
        break;
    case BNE:
        code = 0xD0;
        break;
    case BPL:
        code = 0x10;
        break;
    case BVC:
        code = 0x50;
        break;
    case BVS:
        code = 0x70;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }
    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    uint16_t address;
    int returnVal = resolveAddress(branch->data.relativeMode.address, labelTable, programOffset, &address);
    int offsetValue = (address - ((instructionOffset + programOffset) + 2));
    if (returnVal != SUCCESS)
    {
        return returnVal;
    }

    if (abs(offsetValue) > 127)
    {
        return BRANCHOUTOFRANGE;
    }
    *((signed char *)*outputData) = (signed char)offsetValue;
    *outputData += 1;

    return SUCCESS;
}

int resolveZeroPageInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.zeroPageMode.opcode)
    {
    case ADC:
        code = 0x65;
        break;
    case AND:
        code = 0x25;
        break;
    case BIT:
        code = 0x24;
        break;
    case ASL:
        code = 0x06;
        break;
    case CMP:
        code = 0xC5;
        break;
    case CPX:
        code = 0xE4;
        break;
    case CPY:
        code = 0xC4;
        break;
    case DEC:
        code = 0xC6;
        break;
    case EOR:
        code = 0x45;
        break;
    case INC:
        code = 0xE6;
        break;
    case LDA:
        code = 0xA5;
        break;
    case LDX:
        code = 0xA6;
        break;
    case LDY:
        code = 0xA4;
        break;
    case LSR:
        code = 0x46;
        break;
    case ORA:
        code = 0x05;
        break;
    case ROL:
        code = 0x26;
        break;
    case ROR:
        code = 0x66;
        break;
    case SBC:
        code = 0xE5;
        break;
    case STA:
        code = 0x85;
        break;
    case STX:
        code = 0x86;
        break;
    case STY:
        code = 0x84;
        break;
    default:
        return FATALGENERATORERROR;
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.zeroPageMode.value;
    *outputData += 1;

    return SUCCESS;
}

int resolveZeroPageXInstruction(astBranch *branch, char **outputData)
{

    uint8_t code = 0;
    switch (branch->data.zeroPageMode.opcode)
    {
    case ADC:
        code = 0x75;
        break;
    case AND:
        code = 0x35;
        break;
    case ASL:
        code = 0x16;
        break;
    case CMP:
        code = 0xD5;
        break;
    case DEC:
        code = 0xD6;
        break;
    case EOR:
        code = 0x55;
        break;
    case INC:
        code = 0xF6;
        break;
    case LDA:
        code = 0xB5;
        break;
    case LDY:
        code = 0xB4;
        break;
    case LSR:
        code = 0x56;
        break;
    case ORA:
        code = 0x15;
        break;
    case ROL:
        code = 0x36;
        break;
    case ROR:
        code = 0x76;
        break;
    case SBC:
        code = 0xF5;
        break;
    case STA:
        code = 0x95;
        break;
    case STY:
        code = 0x94;
        break;
    default:
    return FATALGENERATORERROR;
        break;
    }
    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.zeroPageMode.value;
    *outputData += 1;
    return SUCCESS;
}

int resolveZeroPageYInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.zeroPageMode.opcode)
    {
    case LDX:
        code = 0xB6;
        break;
    case STX:
        code = 0x96;
        break;
    default:
    return FATALGENERATORERROR;
        break;
    }
    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.zeroPageMode.value;
    *outputData += 1;
    return SUCCESS;
}

int resolveIndirectInstruction(astBranch *branch, int programOffset, labelList *labelTable, char **outputData)
{
    int returnVal;
    if (branch->data.indirectMode.opcode == JMP)
    {
        returnVal = resolveAddress(branch->data.absoluteMode.address, labelTable, programOffset, (uint16_t *)*outputData);
        *outputData += 2;
    }else{
        return FATALGENERATORERROR;
    }
    return returnVal;
}

int resolveIndirectXInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.zeroPageMode.opcode)
    {
    case ADC:
        code = 0x61;
        break;
    case AND:
        code = 0x21;
        break;
    case CMP:
        code = 0xC1;
        break;
    case EOR:
        code = 0x41;
        break;
    case LDA:
        code = 0xA1;
        break;
    case ORA:
        code = 0x01;
        break;
    case SBC:
        code = 0xE1;
        break;
    case STA:
        code = 0x81;
        break;
    default:
    return FATALGENERATORERROR;
        break;
    }
    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.indirectRegisterMode.value;
    *outputData += 1;
    return SUCCESS;
}

int resolveIndirectYInstruction(astBranch *branch, char **outputData)
{
    uint8_t code = 0;
    switch (branch->data.zeroPageMode.opcode)
    {
    case ADC:
        code = 0x71;
        break;
    case AND:
        code = 0x31;
        break;
    case CMP:
        code = 0xD1;
        break;
    case EOR:
        code = 0x51;
        break;
    case LDA:
        code = 0xB1;
        break;
    case ORA:
        code = 0x11;
        break;
    case SBC:
        code = 0xF1;
        break;
    case STA:
        code = 0x91;
        break;
    default:
    return FATALGENERATORERROR;
        break;
    }
    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.indirectRegisterMode.value;
    *outputData += 1;
    return SUCCESS;
}

// allocate source dump into output data and return length
int assembleParseTree(ast *tree, int programOffset, char **outputData)
{
    labelList labelTable = (labelList){.content = malloc(sizeof(resolvedLabel)), .capacity = 1, .length = 0};
    int codeSize = fillLabelList(tree, &labelTable);

    *outputData = malloc(codeSize);

    char *canonicalStart = *outputData;
    char *dataHead = *outputData;

    for (int i = 0; i < tree->length; i++)
    {
        astBranch currentBranch = tree->content[i];
        int returnVal;
        switch (currentBranch.branchType)
        {
        case directiveBytes:
            memcpy(dataHead, currentBranch.data.byteDirective.content, currentBranch.data.byteDirective.length);
            dataHead += currentBranch.data.byteDirective.length;
            returnVal = SUCCESS;
            break;
        case directiveWords:
            returnVal = resolveWordList(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        case implied:
            returnVal = resolveImpliedInstruction(&currentBranch, &dataHead);
            break;
        case accumulator:
            returnVal = resolveAccumulatorInstruction(&currentBranch, &dataHead);
            break;
        case immediate:
            returnVal = resolveImmediateInstruction(&currentBranch, &dataHead);
            break;
        case absolute:
            returnVal = resolveAbsoluteInstruction(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        case absoluteX:
            returnVal = resolveAbsoluteXInstruction(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        case absoluteY:
            returnVal = resolveAbsoluteYInstruction(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        case relative:
            returnVal = resolveRelativeInstruction(&currentBranch, (int)(dataHead - canonicalStart), programOffset, &labelTable, &dataHead);
            break;
        case zeroPage:
            returnVal = resolveZeroPageInstruction(&currentBranch, &dataHead);
            break;
        case zeroPageX:
            returnVal = resolveZeroPageXInstruction(&currentBranch, &dataHead);
            break;
        case zeroPageY:
            returnVal = resolveZeroPageYInstruction(&currentBranch, &dataHead);
            break;
        case indirect:
            returnVal = resolveIndirectInstruction(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        case indirectX:
            returnVal = resolveIndirectXInstruction(&currentBranch, &dataHead);
            break;
        case indirectY:
            returnVal = resolveIndirectYInstruction(&currentBranch, &dataHead);
            break;
        case label:
            returnVal = SUCCESS;
            break;
        default:
            returnVal = FATALGENERATORERROR;
            break;
        }

        if (returnVal != SUCCESS)
        {
            printError(returnVal, currentBranch.err);
            exit(-1);
        }
    }

    freeLabelList(&labelTable);
    return codeSize;
}
