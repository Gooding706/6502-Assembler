#include <stdlib.h>
#include <memory.h>

#include <ast.h>
#include <parser.h>

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

uint16_t resolveAdress(address_t addr, labelList *list, int programOffset)
{
    if (addr.tag == unresolvedLabel)
    {
        uint16_t out;
        if (!matchLabel(addr.data.labelText, list, programOffset, &out))
        {
            printf("un-resolved label '%s'", addr.data.labelText);
            exit(-1);
        }
        return out;
    }
    else
    {
        return addr.data.addressLiteral;
    }
}

// TODO: convert endianess for big endian systems
void resolveWordList(astBranch *branch, int programOffset, labelList *labelTable, void **outputData)
{
    for (int i = 0; i < branch->data.wordDirective.length; i++)
    {
        address_t currentWord = branch->data.wordDirective.content[i];
        if (currentWord.tag == label)
        {
            if (!matchLabel(currentWord.data.labelText, labelTable, programOffset, (uint16_t *)(*outputData)))
            {
                printf("un-resolved label '%s'", currentWord.data.labelText);
                exit(-1);
            }
        }
        else
        {
            *((uint16_t *)(*outputData)) = branch->data.wordDirective.content[i].data.addressLiteral;
        }
        *outputData += 2;
    }
}

void resolveImpliedInstruction(astBranch *branch, void **outputData)
{
    uint8_t code;
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
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
}

void resolveAccumulatorInstruction(astBranch *branch, void **outputData)
{
    uint8_t code;
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
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
}

void resolveImmediateInstruction(astBranch *branch, void **outputData)
{
    uint8_t code;
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
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint8_t *)*outputData) = branch->data.immediateMode.value;
    *outputData += 1;
}

void resolveAbsoluteInstruction(astBranch *branch, int programOffset, labelList *labelTable, void **outputData)
{
    uint8_t code;
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
        break;
    }

    *((uint8_t *)*outputData) = code;
    *outputData += 1;
    *((uint16_t *)*outputData) = resolveAdress(branch->data.absoluteMode.address, labelTable, programOffset);
    *outputData += 2;
}

// allocate source dump into output data and return length
int assembleParseTree(ast *tree, int programOffset, void **outputData)
{
    labelList labelTable = (labelList){.content = malloc(sizeof(resolvedLabel)), .capacity = 1, .length = 0};
    int codeSize = fillLabelList(tree, &labelTable);

    *outputData = malloc(codeSize);

    void *dataHead = *outputData;
    for (int i = 0; i < tree->length; i++)
    {
        astBranch currentBranch = tree->content[i];
        switch (currentBranch.branchType)
        {
        case directiveBytes:
            memcpy(dataHead, currentBranch.data.byteDirective.content, currentBranch.data.byteDirective.length);
            dataHead += currentBranch.data.byteDirective.length;
            break;
        case directiveWords:
            resolveWordList(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        case implied:
            resolveImpliedInstruction(&currentBranch, &dataHead);
            break;
        case accumulator:
            resolveAccumulatorInstruction(&currentBranch, &dataHead);
            break;
        case immediate:
            resolveImmediateInstruction(&currentBranch, &dataHead);
            break;
        case absolute:
            resolveAbsoluteInstruction(&currentBranch, programOffset, &labelTable, &dataHead);
            break;
        default:
            break;
        }
    }
}