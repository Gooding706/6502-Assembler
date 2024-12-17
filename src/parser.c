#include <stdbool.h>
#include <string.h>

#include <tokens.h>
#include <lexer.h>
#include <ast.h>

#include <parser.h>

bool isOpcode(unsigned short id)
{
    return (id >= ADC && id <= TYA);
}

int expectLineEnd(token **lineStart)
{
    if ((*lineStart)->tokenId == NEWLINE)
    {
        return SUCCESS;
    }
    return MISSINGNEWLINE;
}

char *takeLabelTokenValue(token *t)
{
    char *out = t->textContent;
    t->textContent = NULL;
    return out;
}

int parseLabel(token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == COLON)
    {
        astBranch labelObj = (astBranch){.branchType = label, .data.labelText = takeLabelTokenValue(*lineStart - 1), .err = NULL};

        pushBranch(labelObj, branches);
        *lineStart += 1;
        return expectLineEnd(lineStart);
    }

    return MISSINGCOLON;
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

int readByteAny(token **lineStart, uint8_t *location)
{
    switch ((*lineStart)->tokenId)
    {
    case DOLLAR:
        *lineStart += 1;
        if ((*lineStart)->tokenId != HEXNUMBER_8)
        {
            return MISSINGHEXBYTE;
        }
        *location = takeByteTokenValue(*lineStart);
        return SUCCESS;
    case PERCENT:
        *lineStart += 1;
        if ((*lineStart)->tokenId != BINARYNUMBER_8)
        {
            return MISSINGBINBYTE;
        }
        *location = takeByteTokenValue(*lineStart);
        return SUCCESS;
    case NUMBER_8:
        *location = takeByteTokenValue(*lineStart);
        return SUCCESS;
    default:
        location = NULL;
        return MISSINGBYTETYPE;
    }
}

int parseByteList(token **lineStart, ast *branches)
{
    u8List byteList = {.content = malloc(1), .capacity = 1, .length = 0};
    bool expectComma = false;
    while ((*lineStart)->tokenId != NEWLINE)
    {
        if (expectComma && (*lineStart)->tokenId != COMMA)
        {
            return MISSINGCOMMA;
        }
        else if (!expectComma)
        {
            pushU8(0, &byteList);
            int returnValue = readByteAny(lineStart, &byteList.content[byteList.length - 1]);
            if (returnValue != SUCCESS)
            {
                return returnValue;
            }
        }

        expectComma = !expectComma;
        *lineStart += 1;
    }

    astBranch outBranch = (astBranch){.branchType = directiveBytes, .data.byteDirective = byteList, .err = NULL};

    pushBranch(outBranch, branches);

    return SUCCESS;
}

int readWordAny(token **lineStart, address_t *location)
{
    switch ((*lineStart)->tokenId)
    {
    case DOLLAR:
        *lineStart += 1;
        if ((*lineStart)->tokenId != HEXNUMBER_16)
        {
            return MISSINGHEXWORD;
        }
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return SUCCESS;
    case PERCENT:
        *lineStart += 1;
        if ((*lineStart)->tokenId != BINARYNUMBER_16)
        {
            return MISSINGBINWORD;
        }
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return SUCCESS;
    case NUMBER_16:
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return SUCCESS;
    case NUMBER_8:
    {
        // 8 bit numeric literals can be interpreted as 16 bit
        uint16_t extended = takeByteTokenValue(*lineStart);

        location->tag = constant;
        location->data.addressLiteral = extended;
    }
        return SUCCESS;
    case LABEL:
        location->tag = unresolvedLabel;
        location->data.labelText = takeLabelTokenValue(*lineStart);
        return SUCCESS;

    default:
        location = NULL;
        return MISSINGWORDTYPE;
    }
}

int parseWordList(token **lineStart, ast *branches)
{
    u16List wordList = {.content = malloc(sizeof(address_t)), .capacity = 1, .length = 0};
    bool expectComma = false;
    while ((*lineStart)->tokenId != NEWLINE)
    {
        if (expectComma && (*lineStart)->tokenId != COMMA)
        {
            return MISSINGCOMMA;
        }
        else if (!expectComma)
        {
            pushU16(0, &wordList);
            int returnValue = readWordAny(lineStart, &wordList.content[wordList.length - 1]);
            if (returnValue != SUCCESS)
            {
                return returnValue;
            }
        }

        expectComma = !expectComma;
        *lineStart += 1;
    }

    astBranch outBranch = (astBranch){.branchType = directiveWords, .data.wordDirective = wordList, .err = NULL};

    pushBranch(outBranch, branches);

    return SUCCESS;
}

int parseDirective(token **lineStart, ast *branches)
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
    return MISSINGDIRECTIVE;
}
int parseRelativeInstruction(unsigned short id, token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == LABEL)
    {
        relativeInstruction instr = {.opcode = id};
        instr.address = (address_t){.tag = unresolvedLabel, .data.labelText = takeLabelTokenValue(*lineStart)};
        astBranch newBranch = (astBranch){.branchType = relative, .data.relativeMode = instr, .err = NULL};

        pushBranch(newBranch, branches);

        *lineStart += 1;
        return expectLineEnd(lineStart);
    }
    else
    {
        return MISSINGLABEL;
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
int parseAbsoluteInstruction(unsigned short id, token **lineStart, ast *branches)
{
    address_t val;
    int returnValue = readWordAny(lineStart, &val);
    if (returnValue != SUCCESS)
    {
        return returnValue;
    }

    *lineStart += 1;
    absoluteInstruction instr = (absoluteInstruction){.opcode = id, .address = val};
    if ((*lineStart)->tokenId == NEWLINE)
    {
        if (!isAbsoluteAddressable(id))
        {
            return NOTABSOLUTEADDRESSABLE;
        }
        pushBranch((astBranch){.branchType = absolute, .data.absoluteMode = instr, .err = NULL}, branches);

        return SUCCESS;
    }
    else if ((*lineStart)->tokenId == COMMA)
    {

        *lineStart += 1;
        if ((*lineStart)->tokenId == XREGISTER)
        {
            if (!isAbsoluteXAddressable(id))
            {
                return NOTABSOLUTEXADDRESSABLE;
            }
            pushBranch((astBranch){.branchType = absoluteX, .data.absoluteMode = instr, .err = NULL}, branches);
        }
        else if ((*lineStart)->tokenId == YREGISTER)
        {
            if (!isAbsoluteYAddressable(id))
            {
                return NOTABSOLUTEYADDRESSABLE;
            }
            pushBranch((astBranch){.branchType = absoluteY, .data.absoluteMode = instr, .err = NULL}, branches);
        }
        else
        {
            return MISSINGREGISTERLABEL;
        }

        *lineStart += 1;
        return expectLineEnd(lineStart);
    }

    return MISSINGNEWLINE;
}

int parseZeroPageInstruction(unsigned short id, token **lineStart, ast *branches)
{
    uint8_t val;
    int returnValue = readByteAny(lineStart, &val);
    if (returnValue != SUCCESS)
    {
        return returnValue;
    }
    *lineStart += 1;
    zeroPageInstruction instr = (zeroPageInstruction){.opcode = id, .value = val};
    if ((*lineStart)->tokenId == NEWLINE)
    {
        if (!isZeroPageAddressable(id))
        {
            return NOTZEROPAGEADDRESSABLE;
        }
        pushBranch((astBranch){.branchType = zeroPage, .data.zeroPageMode = instr, .err = NULL}, branches);

        return SUCCESS;
    }
    else if ((*lineStart)->tokenId == COMMA)
    {
        *lineStart += 1;
        if ((*lineStart)->tokenId == XREGISTER)
        {
            if (!isZeroPageXAddressable(id))
            {
                return NOTZEROPAGEYADDRESSABLE;
            }
            pushBranch((astBranch){.branchType = zeroPageX, .data.zeroPageMode = instr, .err = NULL}, branches);
        }
        else if ((*lineStart)->tokenId == YREGISTER)
        {
            if (!isZeroPageYAddressable(id))
            {
                return NOTZEROPAGEYADDRESSABLE;
            }
            pushBranch((astBranch){.branchType = zeroPageY, .data.zeroPageMode = instr, .err = NULL}, branches);
        }
        else
        {
            return MISSINGREGISTERLABEL;
        }

        *lineStart += 1;
        return expectLineEnd(lineStart);
    }

    return MISSINGNEWLINE;
}

int parseImmediateInstruction(unsigned short id, token **lineStart, ast *branches)
{
    uint8_t val;
    int returnValue = readByteAny(lineStart, &val);
    if (returnValue != SUCCESS)
    {
        return returnValue;
    }

    if (!isImmediateAddressable(id))
    {
        return NOTIMMEDIATEADDRESSABLE;
    }

    immediateInstruction instr = (immediateInstruction){.opcode = id, .value = val};
    pushBranch((astBranch){.branchType = immediate, .data.immediateMode = instr, .err = NULL}, branches);

    *lineStart += 1;
    return expectLineEnd(lineStart);
}

int parseRegisterIndirectInstruction(unsigned short id, token **lineStart, ast *branches)
{
    uint8_t val;
    readByteAny(lineStart, &val);
    *lineStart += 1;

    // y indirect
    if ((*lineStart)->tokenId == CLOSEPAREN)
    {
        if (!isIndirectYAddressable(id))
        {
            return NOTINDIRECTYADDRESSABLE;
        }
        *lineStart += 1;
        if ((*lineStart)->tokenId == COMMA && ((*(lineStart)) + 1)->tokenId == YREGISTER)
        {
            indirectRegisterInstruction instr = (indirectRegisterInstruction){.opcode = id, .value = val};
            pushBranch((astBranch){.branchType = indirectY, .data.indirectRegisterMode = instr, .err = NULL}, branches);

            *lineStart += 2;
            return expectLineEnd(lineStart);
        }
        else
        {
            return MISSINGYREGISTER;
        }
    }
    // x indirect
    else if ((*lineStart)->tokenId == COMMA)
    {
        if (!isIndirectXAddressable(id))
        {
            return NOTINDIRECTXADDRESSABLE;
        }
        *lineStart += 1;

        if ((*lineStart)->tokenId == XREGISTER && ((*(lineStart)) + 1)->tokenId == CLOSEPAREN)
        {
            indirectRegisterInstruction instr = (indirectRegisterInstruction){.opcode = id, .value = val};
            pushBranch((astBranch){.branchType = indirectX, .data.indirectRegisterMode = instr, .err = NULL}, branches);

            *lineStart += 2;
            return expectLineEnd(lineStart);
        }
        else
        {
            return MISSINGXREGISTER;
        }
    }

    return MISSINGREGISTERLABEL;
}

int parseIndirectInstruction(unsigned short id, token **lineStart, ast *branches)
{
    if (isWordType(lineStart))
    {
        if (!isIndirectAddressable(id))
        {
            return NOTINDIRECTADDRESSABLE;
        }

        address_t val;
        readWordAny(lineStart, &val);
        *lineStart += 1;

        if ((*lineStart)->tokenId == CLOSEPAREN)
        {
            indirectInstruction instr = (indirectInstruction){.opcode = id, .address = val};
            pushBranch((astBranch){.branchType = indirect, .data.indirectMode = instr, .err = NULL}, branches);

            *lineStart += 1;
            return expectLineEnd(lineStart);
        }
    }
    else if (isByteType(lineStart))
    {
        return parseRegisterIndirectInstruction(id, lineStart, branches);
    }

    return MISSINGCLOSEPAREN;
}

int parseOpcode(unsigned short id, token **lineStart, ast *branches)
{
    if (isImpliedAddressable(id))
    {
        astBranch outBranch = (astBranch){.branchType = implied, .data.opcodeOnly = id, .err = NULL};

        pushBranch(outBranch, branches);
        return expectLineEnd(lineStart);
    }
    else if (isAccumulatorAddressable(id))
    {
        astBranch outBranch = (astBranch){.branchType = accumulator, .data.opcodeOnly = id, .err = NULL};

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

    return UNABLETOINFERADDRESSINGMODE;
}

int parseLine(token **lineStart, ast *branches)
{
    int returnVal = INVALIDELINESTART;
    if ((*lineStart)->tokenId == LABEL)
    {
        *lineStart += 1;
        returnVal = parseLabel(lineStart, branches);
    }
    else if ((*lineStart)->tokenId == PERIOD)
    {
        *lineStart += 1;
        returnVal = parseDirective(lineStart, branches);
    }
    else if (isOpcode((*lineStart)->tokenId))
    {
        unsigned short opcodeId = (*lineStart)->tokenId;
        *lineStart += 1;
        returnVal = parseOpcode(opcodeId, lineStart, branches);
    }

    branches->content[branches->length - 1].err = (*lineStart)->err;
    (*lineStart)->err = NULL;
    return returnVal;
}

bool parseTokenList(tokenList *tokens, ast** out)
{
    *out = malloc(sizeof(ast));
    **out = (ast){.content = malloc(sizeof(astBranch)), .capacity = 1, .length = 0};

    token *currentToken = tokens->content;
    while (currentToken->tokenId != FILEEND)
    {
        int returnValue = parseLine(&currentToken, *out);
        if (returnValue != SUCCESS)
        {
            printError(returnValue, currentToken->err);
            return false;
        }
        currentToken++;
    }

    return true;
}
