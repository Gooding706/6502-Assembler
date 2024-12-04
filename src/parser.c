#include <stdbool.h>
#include <string.h>

#include <tokens.h>
#include <lexer.h>
#include <ast.h>

bool isOpcode(unsigned short id)
{
    return (id > ADC && id < TYA);
}

bool expectLineEnd(token *lineStart)
{
    return (lineStart->tokenId == NEWLINE);
}

char* textCopy(char* start, int length)
{
    return NULL;
}

bool parseLabel(token *lineStart, ast *branches)
{
    if (lineStart->tokenId == COLON)
    {
        int len = strlen(lineStart->textContent);
        astBranch labelObj = (astBranch){.branchType = label, .data.labelText = textCopy(lineStart->textContent, len)};
        pushBranch(labelObj, branches);

        lineStart++;
        return expectLineEnd(lineStart);
    }

    return false;
}

// take the value from a byte token and free any allocated data
uint8_t takeTokenValue(token *t)
{
    uint8_t out = *((uint8_t *)(t->textContent));
    free(t->textContent);
    t->textContent = NULL;
    return out;
}

bool readByteAny(token *lineStart, uint8_t *location)
{
    if (lineStart->tokenId == DOLLAR)
    {
        lineStart++;
        if (lineStart->tokenId != HEXNUMBER_8)
        {
            return false;
        }
        *location = takeTokenValue(lineStart);
        return true;
    }
    else if (lineStart->tokenId == PERCENT)
    {
        lineStart++;
        if (lineStart->tokenId != BINARYNUMBER_8)
        {
            return false;
        }
        *location = takeTokenValue(lineStart);
        return true;
    }
    else if (lineStart->tokenId == NUMBER_8)
    {
        *location = takeTokenValue(lineStart);
        return true;
    }

    return false;
}

bool parseByteList(token *lineStart, ast *branches)
{
    u8List byteList = {.content = malloc(1), .capacity = 1, .length = 0};
    bool expectComma = false;
    while (lineStart->tokenId != NEWLINE)
    {
        if (expectComma && lineStart->tokenId != COMMA)
        {
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
        lineStart++;
    }

    astBranch outBranch = (astBranch){.branchType = directiveBytes, .data.byteDirective = byteList};
    pushBranch(outBranch, branches);

    return true;
}

bool parseWordList(token *lineStart, ast *branches)
{
    u16List adressList = {.content = malloc(sizeof(address_t)), .capacity = 1, .length = 0};
    bool expectComma = false;
    return false;
}

bool parseDirective(token *lineStart, ast *branches)
{
    if (lineStart->tokenId == BYTEDIRECTIVE)
    {
        lineStart++;
        return parseByteList(lineStart, branches);
    }
    else if (lineStart->tokenId == WORDDIRECTIVE)
    {
        lineStart++;
        return parseByteList(lineStart, branches);
    }

    return false;
}

bool parseLine(token *lineStart, ast *branches)
{
    if (lineStart->tokenId == LABEL)
    {
        lineStart++;
        parseLabel(lineStart, branches);
    }
    else if (lineStart->tokenId == PERIOD)
    {
        lineStart++;
        parseDirective(lineStart, branches);
    }
    else if (isOpcode(lineStart->tokenId))
    {
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
        // parse line
    }

    return out;
}