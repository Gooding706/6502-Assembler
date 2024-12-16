#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tokens.h>
#include <ctype.h>
#include <memory.h>

char *loadFile(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        printf("unable to open file '%s'\n", path);
        exit(-1);
    }
    fseek(f, 0L, SEEK_END);
    size_t length = ftell(f);
    rewind(f);
    char *out = malloc(length + 2);
    out[length] = '\n';
       out[length+1] = '\0';
    fread(out, length, 1, f);

    fclose(f);
    return out;
}

bool tokenizeHex(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isxdigit(**tokenStart))
    {
        *tokenStart += 1;
    }

    if ((*tokenStart - canonicalStart) == 4)
    {
        token t = (token){HEXNUMBER_16, malloc(2)};
        *((uint16_t *)t.textContent) = strtoul(canonicalStart, NULL, 16);
        pushToken(t, tokens);
        return true;
    }
    else if ((*tokenStart - canonicalStart) == 2)
    {
        token t = (token){HEXNUMBER_8, malloc(1)};
        *((uint8_t *)t.textContent) = strtoul(canonicalStart, NULL, 16);
        pushToken(t, tokens);
        return true;
    }

    return false;
}

bool tokenizeBin(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (**tokenStart == '0' || **tokenStart == '1')
    {
        *tokenStart += 1;
    }

    if ((*tokenStart - canonicalStart) == 16)
    {
        token t = (token){BINARYNUMBER_16, malloc(2)};
        *((uint16_t *)t.textContent) = strtoul(canonicalStart, NULL, 2);
        pushToken(t, tokens);
        return true;
    }
    else if ((*tokenStart - canonicalStart) == 8)
    {
        token t = (token){BINARYNUMBER_8, malloc(1)};
        *((uint8_t *)t.textContent) = strtoul(canonicalStart, NULL, 2);
        pushToken(t, tokens);
        return true;
    }

    return false;
}

bool tokenizeNum(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isnumber(**tokenStart))
    {
        *tokenStart += 1;
    }

    int value = strtoul(canonicalStart, NULL, 10);

    if (value > UINT8_MAX && value < UINT16_MAX)
    {
        token t = (token){NUMBER_16, malloc(2)};
        *((uint16_t *)t.textContent) = value;
        pushToken(t, tokens);
        return true;
    }
    else if (value < UINT8_MAX)
    {
        token t = (token){NUMBER_8, malloc(1)};
        *((uint8_t *)t.textContent) = value;
        pushToken(t, tokens);
        return true;
    }

    return false;
}

bool tokenizeSingleChar(char chr, tokenList *tokens)
{
    switch (tolower(chr))
    {
    case 'x':
        pushToken((token){XREGISTER, NULL}, tokens);
        return true;
    case 'y':
        pushToken((token){YREGISTER, NULL}, tokens);
        return true;
    default:
    {
        char *allocated = malloc(1);
        *allocated = chr;
        pushToken((token){LABEL, allocated}, tokens);
    }
    break;
    }
    return true;
}

bool tokenizeLabel(char *tokenStart, int length, tokenList *tokens)
{
    char *copiedStr = malloc(length + 1);
    copiedStr[length] = '\0';
    memcpy(copiedStr, tokenStart, length);
    token t = (token){LABEL, copiedStr};
    pushToken(t, tokens);
    return true;
}

bool tokenizeOpcode(char *tokenStart, tokenList *tokens)
{
    char upper[] = {toupper(*tokenStart), toupper(*(tokenStart+1)), toupper(*(tokenStart+2))};

    if (!memcmp(upper, "ADC", 3))
    {
        pushToken((token){ADC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "AND", 3))
    {
        pushToken((token){AND, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ASL", 3))
    {
        pushToken((token){ASL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BCC", 3))
    {
        pushToken((token){BCC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BCS", 3))
    {
        pushToken((token){BCS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BEQ", 3))
    {
        pushToken((token){BEQ, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BIT", 3))
    {
        pushToken((token){BIT, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BMI", 3))
    {
        pushToken((token){BMI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BNE", 3))
    {
        pushToken((token){BNE, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BPL", 3))
    {
        pushToken((token){BPL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BRK", 3))
    {
        pushToken((token){BRK, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BVC", 3))
    {
        pushToken((token){BVC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BVS", 3))
    {
        pushToken((token){BVS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLC", 3))
    {
        pushToken((token){CLC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLD", 3))
    {
        pushToken((token){CLD, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLI", 3))
    {
        pushToken((token){CLI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLV", 3))
    {
        pushToken((token){CLV, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CMP", 3))
    {
        pushToken((token){CMP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CPX", 3))
    {
        pushToken((token){CPX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CPY", 3))
    {
        pushToken((token){CPY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "DEC", 3))
    {
        pushToken((token){DEC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "DEX", 3))
    {
        pushToken((token){DEX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "DEY", 3))
    {
        pushToken((token){DEY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "EOR", 3))
    {
        pushToken((token){EOR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "INC", 3))
    {
        pushToken((token){INC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "INX", 3))
    {
        pushToken((token){INX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "INY", 3))
    {
        pushToken((token){INY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "JMP", 3))
    {
        pushToken((token){JMP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "JSR", 3))
    {
        pushToken((token){JSR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LDA", 3))
    {
        pushToken((token){LDA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LDX", 3))
    {
        pushToken((token){LDX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LDY", 3))
    {
        pushToken((token){LDY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LSR", 3))
    {
        pushToken((token){LSR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "NOP", 3))
    {
        pushToken((token){NOP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ORA", 3))
    {
        pushToken((token){ORA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PHA", 3))
    {
        pushToken((token){PHA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PHP", 3))
    {
        pushToken((token){PHP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PLA", 3))
    {
        pushToken((token){PLA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PLP", 3))
    {
        pushToken((token){PLP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ROL", 3))
    {
        pushToken((token){ROL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ROR", 3))
    {
        pushToken((token){ROR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "RTI", 3))
    {
        pushToken((token){RTI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "RTS", 3))
    {
        pushToken((token){RTS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SBC", 3))
    {
        pushToken((token){SBC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SEC", 3))
    {
        pushToken((token){SEC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SED", 3))
    {
        pushToken((token){SED, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SEI", 3))
    {
        pushToken((token){SEI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "STA", 3))
    {
        pushToken((token){STA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "STX", 3))
    {
        pushToken((token){STX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "STY", 3))
    {
        pushToken((token){STY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TAX", 3))
    {
        pushToken((token){TAX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TAY", 3))
    {
        pushToken((token){TAY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TSX", 3))
    {
        pushToken((token){TSX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TXA", 3))
    {
        pushToken((token){TXA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TXS", 3))
    {
        pushToken((token){TXS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TYA", 3))
    {
        pushToken((token){TYA, NULL}, tokens);
        return true;
    }

    return false;
}

bool tokenizeMultiChar(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isalnum(**tokenStart) || **tokenStart == '_')
    {
        *tokenStart += 1;
    }

    if ((*tokenStart - canonicalStart) == 1)
    {
        return tokenizeSingleChar(*canonicalStart, tokens);
    }
    else if ((*tokenStart - canonicalStart) == 3)
    {
        
        return tokenizeOpcode(canonicalStart, tokens);
    }

    return tokenizeLabel(canonicalStart, (*tokenStart - canonicalStart), tokens);
}

bool tokenizeDirective(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isalnum(**tokenStart))
    {
        **tokenStart = tolower(**tokenStart);
        *tokenStart += 1;
    }

    int length = (*tokenStart - canonicalStart);
    if (length == 4)
    {
        if (!memcmp(canonicalStart, "byte", 4))
        {
            pushToken((token){BYTEDIRECTIVE, NULL}, tokens);
            return true;
        }
        else if (!memcmp(canonicalStart, "word", 4))
        {
            pushToken((token){WORDDIRECTIVE, NULL}, tokens);
            return true;
        }
    }
    return false;
}

void readWhiteSpace(char **tokenStart)
{
    while (**tokenStart == ' ' || **tokenStart == '\t' || **tokenStart == '\v')
    {

        *tokenStart += 1;
    }
}

bool tokenize(char **tokenStart, tokenList *tokens)
{
    readWhiteSpace(tokenStart);
 
    switch (**tokenStart)
    {
    case ':':
        pushToken((token){COLON, NULL}, tokens);
        *tokenStart += 1;
        return true;
    case '#':
        pushToken((token){HASH, NULL}, tokens);
        *tokenStart += 1;
        return true;
    case '(':
        pushToken((token){OPENPAREN, NULL}, tokens);
        *tokenStart += 1;
        return true;
    case ')':
        pushToken((token){CLOSEPAREN, NULL}, tokens);
        *tokenStart += 1;
        return true;
    case ',':
        pushToken((token){COMMA, NULL}, tokens);
        *tokenStart += 1;
        return true;
    case '.':
        pushToken((token){PERIOD, NULL}, tokens);
        *tokenStart += 1;
        return tokenizeDirective(tokenStart, tokens);
    case '\n':
        return true;
    case '\r':
        return true;
    case '\0':
        return true;
    case ';':
        return true;
    case '$':
        pushToken((token){DOLLAR, NULL}, tokens);
        *tokenStart += 1;
        return tokenizeHex(tokenStart, tokens);
    case '%':
        pushToken((token){PERCENT, NULL}, tokens);
        *tokenStart += 1;
        return tokenizeBin(tokenStart, tokens);
    default:
        if (isnumber(**tokenStart))
        {
            return tokenizeNum(tokenStart, tokens);
        }
            
        return tokenizeMultiChar(tokenStart, tokens);
    }
}

bool tokenizeLine(char **lineStart, tokenList *tokens)
{
    bool isComment = false;
    while (**lineStart != '\r' && **lineStart != '\n' && **lineStart != '\0')
    {
        if (isComment)
        {
            *lineStart += 1;
            continue;
        }
        else if (**lineStart == ';')
        {
            isComment = true;
            continue;
        }

        if (!tokenize(lineStart, tokens))
        {
            return false;
        }
    }

    return true;
}

void tokenizeFile(char *content, tokenList *tokens)
{
    char *currentChar = content;
    int previousLen = tokens->length;
    while (*currentChar != '\0')
    {
        if (!tokenizeLine(&currentChar, tokens))
        {
           // printf("%s\n", currentChar);
            printf("failed during tokenization\n");
            exit(-1);
        }

        if (previousLen < tokens->length)
        {
            pushToken((token){NEWLINE, NULL}, tokens);
        }

        previousLen = tokens->length;
        currentChar++;
    }


    pushToken((token){FILEEND, NULL}, tokens);
}
