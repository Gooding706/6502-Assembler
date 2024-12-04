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
    char *out = malloc(length);
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
    if (!memcmp(tokenStart, "ADC", 3))
    {
        pushToken((token){ADC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "AND", 3))
    {
        pushToken((token){AND, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "ASL", 3))
    {
        pushToken((token){ASL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BCC", 3))
    {
        pushToken((token){BCC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BCS", 3))
    {
        pushToken((token){BCS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BEQ", 3))
    {
        pushToken((token){BEQ, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BIT", 3))
    {
        pushToken((token){BIT, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BMI", 3))
    {
        pushToken((token){BMI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BNE", 3))
    {
        pushToken((token){BNE, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BPL", 3))
    {
        pushToken((token){BPL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BRK", 3))
    {
        pushToken((token){BRK, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BVC", 3))
    {
        pushToken((token){BVC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "BVS", 3))
    {
        pushToken((token){BVS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CLC", 3))
    {
        pushToken((token){CLC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CLD", 3))
    {
        pushToken((token){CLD, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CLI", 3))
    {
        pushToken((token){CLI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CLV", 3))
    {
        pushToken((token){CLV, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CMP", 3))
    {
        pushToken((token){CMP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CPX", 3))
    {
        pushToken((token){CPX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "CPY", 3))
    {
        pushToken((token){CPY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "DEC", 3))
    {
        pushToken((token){DEC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "DEX", 3))
    {
        pushToken((token){DEX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "DEY", 3))
    {
        pushToken((token){DEY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "EOR", 3))
    {
        pushToken((token){EOR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "INC", 3))
    {
        pushToken((token){INC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "INX", 3))
    {
        pushToken((token){INX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "INY", 3))
    {
        pushToken((token){INY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "JMP", 3))
    {
        pushToken((token){JMP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "JSR", 3))
    {
        pushToken((token){JSR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "LDA", 3))
    {
        pushToken((token){LDA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "LDX", 3))
    {
        pushToken((token){LDX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "LDY", 3))
    {
        pushToken((token){LDY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "LSR", 3))
    {
        pushToken((token){LSR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "NOP", 3))
    {
        pushToken((token){NOP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "ORA", 3))
    {
        pushToken((token){ORA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "PHA", 3))
    {
        pushToken((token){PHA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "PHP", 3))
    {
        pushToken((token){PHP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "PLA", 3))
    {
        pushToken((token){PLA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "PLP", 3))
    {
        pushToken((token){PLP, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "ROL", 3))
    {
        pushToken((token){ROL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "ROR", 3))
    {
        pushToken((token){ROR, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "RTI", 3))
    {
        pushToken((token){RTI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "RTS", 3))
    {
        pushToken((token){RTS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "SBC", 3))
    {
        pushToken((token){SBC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "SEC", 3))
    {
        pushToken((token){SEC, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "SED", 3))
    {
        pushToken((token){SED, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "SEI", 3))
    {
        pushToken((token){SEI, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "STA", 3))
    {
        pushToken((token){STA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "STX", 3))
    {
        pushToken((token){STX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "STY", 3))
    {
        pushToken((token){STY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "TAX", 3))
    {
        pushToken((token){TAX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "TAY", 3))
    {
        pushToken((token){TAY, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "TSX", 3))
    {
        pushToken((token){TSX, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "TXA", 3))
    {
        pushToken((token){TXA, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "TXS", 3))
    {
        pushToken((token){TXS, NULL}, tokens);
        return true;
    }
    else if (!memcmp(tokenStart, "TYA", 3))
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
    while (isspace(**tokenStart) && **tokenStart != '\0' && **tokenStart != '\n')
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
        return tokenizeMultiChar(tokenStart, tokens);
    }
}

bool tokenizeLine(char **lineStart, tokenList *tokens)
{
    bool isComment = false;
    while (**lineStart != '\n' && **lineStart != '\0')
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
            printf("%s\n", currentChar);
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