#include <stdbool.h>
#include <string.h>

#include <tokens.h>
#include <lexer.h>
#include <ast.h>

#include <parser.h>

bool isOpcode(unsigned short id)
{
    return (id > ADC && id < TYA);
}

bool expectLineEnd(token **lineStart)
{
    return ((*lineStart)->tokenId == NEWLINE);
}

char *takeLabelTokenValue(token *t)
{
    char *out = t->textContent;
    t->textContent = NULL;
    return out;
}

bool parseLabel(token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == COLON)
    {
        astBranch labelObj = (astBranch){.branchType = label, .data.labelText = takeLabelTokenValue(*lineStart - 1)};
        pushBranch(labelObj, branches);
        *lineStart += 1;
        return expectLineEnd(lineStart);
    }

    printf("Expected COLON\n");
    return false;
}

// take the value from a byte token and free any allocated data
uint8_t takeByteTokenValue(token *t)
{
    uint8_t out = *((uint8_t *)(t->textContent));
    free(t->textContent);
    t->textContent = NULL;
    return out;
}

uint16_t takeWordTokenValue(token *t)
{
    uint16_t out = *((uint16_t *)(t->textContent));
    free(t->textContent);
    t->textContent = NULL;
    return out;
}

bool readByteAny(token **lineStart, uint8_t *location)
{
    switch ((*lineStart)->tokenId)
    {
    case DOLLAR:
        *lineStart += 1;
        if ((*lineStart)->tokenId != HEXNUMBER_8)
        {
            printf("Expected HEX8\n");
            return false;
        }
        *location = takeByteTokenValue(*lineStart);
        return true;
    case PERCENT:
        *lineStart += 1;
        if ((*lineStart)->tokenId != BINARYNUMBER_8)
        {
            printf("Expected BINARY8\n");
            return false;
        }
        *location = takeByteTokenValue(*lineStart);
        return true;
    case NUMBER_8:
        *location = takeByteTokenValue(*lineStart);
        return true;
    default:
        location = NULL;
        return false;
    }
}

bool parseByteList(token **lineStart, ast *branches)
{
    u8List byteList = {.content = malloc(1), .capacity = 1, .length = 0};
    bool expectComma = false;
    while ((*lineStart)->tokenId != NEWLINE)
    {
        if (expectComma && (*lineStart)->tokenId != COMMA)
        {
            printf("Expected COMMA\n");
            return false;
        }
        else if (!expectComma)
        {
            pushU8(0, &byteList);
            if (!readByteAny(lineStart, &byteList.content[byteList.length - 1]))
            {
                return false;
            }
        }

        expectComma = !expectComma;
        *lineStart += 1;
    }

    astBranch outBranch = (astBranch){.branchType = directiveBytes, .data.byteDirective = byteList};
    pushBranch(outBranch, branches);

    return true;
}

bool readWordAny(token **lineStart, address_t *location)
{
    switch ((*lineStart)->tokenId)
    {
    case DOLLAR:
        *lineStart += 1;
        if ((*lineStart)->tokenId != HEXNUMBER_16)
        {
            printf("Expected HEX16\n");
            return false;
        }
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return true;
    case PERCENT:
        *lineStart += 1;
        if ((*lineStart)->tokenId != BINARYNUMBER_16)
        {
            printf("Expected BINARY16\n");
            return false;
        }
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return true;
    case NUMBER_16:
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return true;
    case NUMBER_8:
    {
        // 8 bit numeric literals can be interpreted as 16 bit
        uint16_t extended = takeByteTokenValue(*lineStart);

        location->tag = constant;
        location->data.addressLiteral = extended;
    }
        return true;
    case LABEL:
        location->tag = unresolvedLabel;
        location->data.labelText = takeLabelTokenValue(*lineStart);
        return true;

    default:
        location = NULL;
        return false;
    }
}

bool parseWordList(token **lineStart, ast *branches)
{
    u16List wordList = {.content = malloc(sizeof(address_t)), .capacity = 1, .length = 0};
    bool expectComma = false;
    while ((*lineStart)->tokenId != NEWLINE)
    {
        if (expectComma && (*lineStart)->tokenId != COMMA)
        {
            printf("Expected COMMA\n");
            return false;
        }
        else if (!expectComma)
        {
            pushU16(0, &wordList);
            if (!readWordAny(lineStart, &wordList.content[wordList.length - 1]))
            {
                return false;
            }
        }

        expectComma = !expectComma;
        *lineStart += 1;
    }

    astBranch outBranch = (astBranch){.branchType = directiveWords, .data.wordDirective = wordList};
    pushBranch(outBranch, branches);

    return true;
}

bool parseDirective(token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == BYTEDIRECTIVE)
    {
        *lineStart += 1;
        return parseByteList(lineStart, branches);
    }
    else if ((*lineStart)->tokenId == WORDDIRECTIVE)
    {
        *lineStart += 1;
        return parseWordList(lineStart, branches);
    }
    printf("Expected DIRECTIVE\n");
    return false;
}
bool parseRelativeInstruction(unsigned short id, token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == LABEL)
    {
        relativeInstruction instr = {.opcode = id};
        instr.address = (address_t){.tag = unresolvedLabel, .data.labelText = takeLabelTokenValue(*lineStart)};
        astBranch newBranch = (astBranch){.branchType = relative, .data.relativeMode = instr};
        pushBranch(newBranch, branches);

        *lineStart += 1;
        return expectLineEnd(lineStart);
    }
    else
    {
        return false;
    }
}

bool isWordType(token **lineStart)
{
    if ((*lineStart)->tokenId == DOLLAR)
    {
        return (((*lineStart) + 1)->tokenId == HEXNUMBER_16);
    }
    else if ((*lineStart)->tokenId == PERCENT)
    {
        return (((*lineStart) + 1)->tokenId == BINARYNUMBER_16);
    }
    else if ((*lineStart)->tokenId == NUMBER_16)
    {
        return true;
    }
    else if ((*lineStart)->tokenId == LABEL)
    {
        return true;
    }
    return false;
}

bool isByteType(token **lineStart)
{
    if ((*lineStart)->tokenId == DOLLAR)
    {
        return (((*lineStart) + 1)->tokenId == HEXNUMBER_8);
    }
    else if ((*lineStart)->tokenId == PERCENT)
    {
        return (((*lineStart) + 1)->tokenId == BINARYNUMBER_8);
    }
    else if ((*lineStart)->tokenId == NUMBER_8)
    {
        return true;
    }
    return false;
}

// TODO: not a fan of the program flow within this function
bool parseAbsoluteInstruction(unsigned short id, token **lineStart, ast *branches)
{
    address_t val;
    if (!readWordAny(lineStart, &val))
    {
        return false;
    }

    *lineStart += 1;
    absoluteInstruction instr = (absoluteInstruction){.opcode = id, .address = val};
    if ((*lineStart)->tokenId == NEWLINE && isAbsoluteAddressable(id))
    {
        pushBranch((astBranch){.branchType = absolute, .data.absoluteMode = instr}, branches);
        return true;
    }
    else if ((*lineStart)->tokenId == COMMA)
    {

        *lineStart += 1;
        if ((*lineStart)->tokenId == XREGISTER && isAbsoluteXAddressable(id))
        {
            pushBranch((astBranch){.branchType = absoluteX, .data.absoluteMode = instr}, branches);
        }
        else if ((*lineStart)->tokenId == YREGISTER && isAbsoluteYAddressable(id))
        {
            pushBranch((astBranch){.branchType = absoluteY, .data.absoluteMode = instr}, branches);
        }
        else
        {
            return false;
        }

        *lineStart += 1;
        return expectLineEnd(lineStart);
    }

    return false;
}

bool parseZeroPageInstruction(unsigned short id, token **lineStart, ast *branches)
{
    uint8_t val;
    if (!readByteAny(lineStart, &val))
    {
        return false;
    }
    *lineStart += 1;
    zeroPageInstruction instr = (zeroPageInstruction){.opcode = id, .value = val};
    if ((*lineStart)->tokenId == NEWLINE && isZeroPageAddressable(id))
    {
        pushBranch((astBranch){.branchType = zeroPage, .data.zeroPageMode = instr}, branches);
        return true;
    }
    else if ((*lineStart)->tokenId == COMMA)
    {
        *lineStart += 1;
        if ((*lineStart)->tokenId == XREGISTER && isZeroPageXAddressable(id))
        {
            pushBranch((astBranch){.branchType = zeroPageX, .data.zeroPageMode = instr}, branches);
        }
        else if ((*lineStart)->tokenId == YREGISTER && isZeroPageYAddressable(id))
        {
            pushBranch((astBranch){.branchType = zeroPageY, .data.zeroPageMode = instr}, branches);
        }
        else
        {
            return false;
        }

        *lineStart += 1;
        return expectLineEnd(lineStart);
    }

    return false;
}

bool parseImmediateInstruction(unsigned short id, token **lineStart, ast *branches)
{
    uint8_t val;
    if (!readByteAny(lineStart, &val) || !(isImmediateAddressable(id)))
    {
        return false;
    }

    immediateInstruction instr = (immediateInstruction){.opcode = id, .value = val};
    pushBranch((astBranch){.branchType = immediate, .data.immediateMode = instr}, branches);

    *lineStart += 1;
    return expectLineEnd(lineStart);
}

bool parseRegisterIndirectInstruction(unsigned short id, token **lineStart, ast *branches)
{
    uint8_t val;
    readByteAny(lineStart, &val);
    *lineStart += 1;

    // y indirect
    if ((*lineStart)->tokenId == CLOSEPAREN && isIndirectYAddressable(id))
    {
        *lineStart += 1;
        if ((*lineStart)->tokenId == COMMA && ((*(lineStart))+1)->tokenId == YREGISTER)
        {
            indirectRegisterInstruction instr = (indirectRegisterInstruction){.opcode = id, .value = val};
            pushBranch((astBranch){.branchType = indirectY, .data.indirectRegisterMode = instr}, branches);

            *lineStart += 2;
            return expectLineEnd(lineStart);
        }
        else
        {
            return false;
        }
    }
    // x indirect
    else if ((*lineStart)->tokenId == COMMA && isIndirectXAddressable(id))
    {
        *lineStart += 1;

        if ((*lineStart)->tokenId == XREGISTER && ((*(lineStart))+1)->tokenId == CLOSEPAREN)
        {
            indirectRegisterInstruction instr = (indirectRegisterInstruction){.opcode = id, .value = val};
            pushBranch((astBranch){.branchType = indirectX, .data.indirectRegisterMode = instr}, branches);

            *lineStart += 2;
            return expectLineEnd(lineStart);
        }
        else
        {
            printf("Hai\n");
            return false;
        }
    }

    return false;
}

bool parseIndirectInstruction(unsigned short id, token **lineStart, ast *branches)
{
    if (isWordType(lineStart) && isIndirectAddressable(id))
    {
        address_t val;
        readWordAny(lineStart, &val);
        *lineStart += 1;

        if ((*lineStart)->tokenId == CLOSEPAREN)
        {
            indirectInstruction instr = (indirectInstruction){.opcode = id, .address = val};
            pushBranch((astBranch){.branchType = indirect, .data.indirectMode = instr}, branches);

            *lineStart += 1;
            return expectLineEnd(lineStart);
        }
    }
    else if (isByteType(lineStart))
    {
        return parseRegisterIndirectInstruction(id, lineStart, branches);
    }

    return false;
}

bool parseOpcode(unsigned short id, token **lineStart, ast *branches)
{
    if (isImpliedAddressable(id))
    {
        astBranch outBranch = (astBranch){.branchType = implied, .data.opcodeOnly = id};
        pushBranch(outBranch, branches);
        return expectLineEnd(lineStart);
    }
    else if (isAccumulatorAddressable(id))
    {
        astBranch outBranch = (astBranch){.branchType = accumulator, .data.opcodeOnly = id};
        pushBranch(outBranch, branches);
        return expectLineEnd(lineStart);
    }
    else if (isRelativeAddressable(id))
    {
        return parseRelativeInstruction(id, lineStart, branches);
    }
    else if (isWordType(lineStart))
    {
        return parseAbsoluteInstruction(id, lineStart, branches);
    }
    else if (isByteType(lineStart))
    {
        return parseZeroPageInstruction(id, lineStart, branches);
    }
    else if ((*lineStart)->tokenId == HASH)
    {
        *lineStart += 1;
        return parseImmediateInstruction(id, lineStart, branches);
    }
    else if ((*lineStart)->tokenId == OPENPAREN)
    {
        *lineStart += 1;
        return parseIndirectInstruction(id, lineStart, branches);
    }

    return false;
}

bool parseLine(token **lineStart, ast *branches)
{

    if ((*lineStart)->tokenId == LABEL)
    {
        *lineStart += 1;
        return parseLabel(lineStart, branches);
    }
    else if ((*lineStart)->tokenId == PERIOD)
    {
        *lineStart += 1;
        return parseDirective(lineStart, branches);
    }
    else if (isOpcode((*lineStart)->tokenId))
    {
        unsigned short opcodeId = (*lineStart)->tokenId;
        *lineStart += 1;
        return parseOpcode(opcodeId, lineStart, branches);
    }

    return false;
}

ast *parseTokenList(tokenList *tokens)
{
    ast *out = malloc(sizeof(ast));
    *out = (ast){.content = malloc(sizeof(astBranch)), .capacity = 1, .length = 0};

    token *currentToken = tokens->content;
    while (currentToken->tokenId != FILEEND)
    {
        if (!parseLine(&currentToken, out))
        {
            printf("%i", (int)currentToken->tokenId);
            printf("failed during parsing!\n");
            exit(-1);
        }
        currentToken++;
    }

    return out;
}
