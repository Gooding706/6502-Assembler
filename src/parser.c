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
        *lineStart +=1;
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
    if ((*lineStart)->tokenId == DOLLAR)
    {
        *lineStart +=1;
        if ((*lineStart)->tokenId != HEXNUMBER_8)
        {
            printf("Expected HEX8\n");
            return false;
        }
        *location = takeByteTokenValue(*lineStart);
        return true;
    }
    else if ((*lineStart)->tokenId == PERCENT)
    {
        *lineStart +=1;
        if ((*lineStart)->tokenId != BINARYNUMBER_8)
        {
            printf("Expected BINARY8\n");
            return false;
        }
        *location = takeByteTokenValue(*lineStart);
        return true;
    }
    else if ((*lineStart)->tokenId == NUMBER_8)
    {
        *location = takeByteTokenValue(*lineStart);
        return true;
    }

    location = NULL;
    return false;
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
        *lineStart +=1;
    }

    astBranch outBranch = (astBranch){.branchType = directiveBytes, .data.byteDirective = byteList};
    pushBranch(outBranch, branches);

    return true;
}

bool readWordAny(token **lineStart, address_t *location)
{
    // TODO: why is this not a switch statement?
    if ((*lineStart)->tokenId == DOLLAR)
    {
        *lineStart +=1;
        if ((*lineStart)->tokenId != HEXNUMBER_16)
        {
            printf("Expected HEX16\n");
            return false;
        }
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return true;
    }
    else if ((*lineStart)->tokenId == PERCENT)
    {
        *lineStart +=1;
        if ((*lineStart)->tokenId != BINARYNUMBER_16)
        {
            printf("Expected BINARY16\n");
            return false;
        }
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return true;
    }
    else if ((*lineStart)->tokenId == NUMBER_16)
    {
        location->tag = constant;
        location->data.addressLiteral = takeWordTokenValue(*lineStart);
        return true;
    }
    else if ((*lineStart)->tokenId == NUMBER_8)
    {
        // 8 bit numeric literals can be interpreted as 16 bit
        uint16_t extended = takeByteTokenValue(*lineStart);

        location->tag = constant;
        location->data.addressLiteral = extended;
        return true;
    }
    else if ((*lineStart)->tokenId == LABEL)
    {
        location->tag = unresolvedLabel;
        location->data.labelText = takeLabelTokenValue(*lineStart);
        return true;
    }

    location = NULL;
    return false;
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
        *lineStart +=1;
    }

    astBranch outBranch = (astBranch){.branchType = directiveWords, .data.wordDirective = wordList};
    pushBranch(outBranch, branches);

    return true;
}

bool parseDirective(token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == BYTEDIRECTIVE)
    {
        *lineStart +=1;
        return parseByteList(lineStart, branches);
    }
    else if ((*lineStart)->tokenId == WORDDIRECTIVE)
    {
        *lineStart +=1;
        return parseWordList(lineStart, branches);
    }
    printf("Expected DIRECTIVE\n");
    return false;
}

bool parseLine(token **lineStart, ast *branches)
{
    if ((*lineStart)->tokenId == LABEL)
    {
        *lineStart +=1;
        return parseLabel(lineStart, branches);
    }
    else if ((*lineStart)->tokenId == PERIOD)
    {
        *lineStart +=1;
        return parseDirective(lineStart, branches);
    }
    else if (isOpcode((*lineStart)->tokenId))
    {
        printf("Opcodes are not supported\n");
        return false;
    }
    printf("%i\n", (int)((*lineStart)->tokenId));
    return false;
}

// TODO:Better Error Messages
ast *parseTokenList(tokenList *tokens)
{
    ast *out = malloc(sizeof(ast));
    *out = (ast){.content = malloc(sizeof(astBranch)), .capacity = 1, .length = 0};

    token *currentToken = tokens->content;
    while (currentToken->tokenId != FILEEND)
    {
        if (!parseLine(&currentToken, out))
        {
            printf("failed during parsing!\n");
            exit(-1);
        }
        currentToken++;
    }

    return out;
}