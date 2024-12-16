#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tokens.h>
#include <ctype.h>
#include <memory.h>

#include <errors.h>

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

int tokenizeHex(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isxdigit(**tokenStart))
    {
        *tokenStart += 1;
    }

    if ((*tokenStart - canonicalStart) == 4)
    {
        token t = (token){HEXNUMBER_16, malloc(2), NULL};
        *((uint16_t *)t.textContent) = strtoul(canonicalStart, NULL, 16);
        pushToken(t, tokens);
        return SUCCESS;
    }
    else if ((*tokenStart - canonicalStart) == 2)
    {
        token t = (token){HEXNUMBER_8, malloc(1), NULL};
        *((uint8_t *)t.textContent) = strtoul(canonicalStart, NULL, 16);
        pushToken(t, tokens);
        return SUCCESS;
    }

    return BADHEXLENGTH;
}

int tokenizeBin(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (**tokenStart == '0' || **tokenStart == '1')
    {
        *tokenStart += 1;
    }

    if ((*tokenStart - canonicalStart) == 16)
    {
        token t = (token){BINARYNUMBER_16, malloc(2), NULL};
        *((uint16_t *)t.textContent) = strtoul(canonicalStart, NULL, 2);
        pushToken(t, tokens);
        return SUCCESS;
    }
    else if ((*tokenStart - canonicalStart) == 8)
    {
        token t = (token){BINARYNUMBER_8, malloc(1), NULL};
        *((uint8_t *)t.textContent) = strtoul(canonicalStart, NULL, 2);
        pushToken(t, tokens);
        return SUCCESS;
    }

    return BADBINARYLENGTH;
}

int tokenizeNum(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isnumber(**tokenStart))
    {
        *tokenStart += 1;
    }

    int value = strtoul(canonicalStart, NULL, 10);

    if (value > UINT8_MAX && value < UINT16_MAX)
    {
        token t = (token){NUMBER_16, malloc(2), NULL};
        *((uint16_t *)t.textContent) = value;
        pushToken(t, tokens);
        return SUCCESS;
    }
    else if (value < UINT8_MAX)
    {
        token t = (token){NUMBER_8, malloc(1), NULL};
        *((uint8_t *)t.textContent) = value;
        pushToken(t, tokens);
        return SUCCESS;
    }

    return NUMBEROUTOFRANGE;
}

int tokenizeSingleChar(char chr, tokenList *tokens)
{
    switch (tolower(chr))
    {
    case 'x':
        pushToken((token){XREGISTER, NULL, NULL}, tokens);
        return SUCCESS;
    case 'y':
        pushToken((token){YREGISTER, NULL, NULL}, tokens);
        return SUCCESS;
    default:
    {
        char *allocated = malloc(1);
        *allocated = chr;
        pushToken((token){LABEL, allocated, NULL}, tokens);
    }
    break;
    }

    return SUCCESS;
}

int tokenizeLabel(char *tokenStart, int length, tokenList *tokens)
{
    char *copiedStr = malloc(length + 1);
    copiedStr[length] = '\0';
    memcpy(copiedStr, tokenStart, length);
    token t = (token){LABEL, copiedStr, NULL};
    pushToken(t, tokens);
    return SUCCESS;
}

bool tokenizeOpcode(char *tokenStart, tokenList *tokens)
{
    char upper[] = {toupper(*tokenStart), toupper(*(tokenStart+1)), toupper(*(tokenStart+2))};

    if (!memcmp(upper, "ADC", 3))
    {
        pushToken((token){ADC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "AND", 3))
    {
        pushToken((token){AND, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ASL", 3))
    {
        pushToken((token){ASL, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BCC", 3))
    {
        pushToken((token){BCC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BCS", 3))
    {
        pushToken((token){BCS, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BEQ", 3))
    {
        pushToken((token){BEQ, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BIT", 3))
    {
        pushToken((token){BIT, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BMI", 3))
    {
        pushToken((token){BMI, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BNE", 3))
    {
        pushToken((token){BNE, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BPL", 3))
    {
        pushToken((token){BPL, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BRK", 3))
    {
        pushToken((token){BRK, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BVC", 3))
    {
        pushToken((token){BVC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "BVS", 3))
    {
        pushToken((token){BVS, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLC", 3))
    {
        pushToken((token){CLC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLD", 3))
    {
        pushToken((token){CLD, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLI", 3))
    {
        pushToken((token){CLI, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CLV", 3))
    {
        pushToken((token){CLV, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CMP", 3))
    {
        pushToken((token){CMP, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CPX", 3))
    {
        pushToken((token){CPX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "CPY", 3))
    {
        pushToken((token){CPY, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "DEC", 3))
    {
        pushToken((token){DEC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "DEX", 3))
    {
        pushToken((token){DEX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "DEY", 3))
    {
        pushToken((token){DEY, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "EOR", 3))
    {
        pushToken((token){EOR, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "INC", 3))
    {
        pushToken((token){INC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "INX", 3))
    {
        pushToken((token){INX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "INY", 3))
    {
        pushToken((token){INY, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "JMP", 3))
    {
        pushToken((token){JMP, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "JSR", 3))
    {
        pushToken((token){JSR, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LDA", 3))
    {
        pushToken((token){LDA, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LDX", 3))
    {
        pushToken((token){LDX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LDY", 3))
    {
        pushToken((token){LDY, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "LSR", 3))
    {
        pushToken((token){LSR, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "NOP", 3))
    {
        pushToken((token){NOP, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ORA", 3))
    {
        pushToken((token){ORA, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PHA", 3))
    {
        pushToken((token){PHA, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PHP", 3))
    {
        pushToken((token){PHP, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PLA", 3))
    {
        pushToken((token){PLA, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "PLP", 3))
    {
        pushToken((token){PLP, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ROL", 3))
    {
        pushToken((token){ROL, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "ROR", 3))
    {
        pushToken((token){ROR, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "RTI", 3))
    {
        pushToken((token){RTI, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "RTS", 3))
    {
        pushToken((token){RTS, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SBC", 3))
    {
        pushToken((token){SBC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SEC", 3))
    {
        pushToken((token){SEC, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SED", 3))
    {
        pushToken((token){SED, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "SEI", 3))
    {
        pushToken((token){SEI, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "STA", 3))
    {
        pushToken((token){STA, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "STX", 3))
    {
        pushToken((token){STX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "STY", 3))
    {
        pushToken((token){STY, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TAX", 3))
    {
        pushToken((token){TAX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TAY", 3))
    {
        pushToken((token){TAY, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TSX", 3))
    {
        pushToken((token){TSX, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TXA", 3))
    {
        pushToken((token){TXA, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TXS", 3))
    {
        pushToken((token){TXS, NULL, NULL}, tokens);
        return true;
    }
    else if (!memcmp(upper, "TYA", 3))
    {
        pushToken((token){TYA, NULL, NULL}, tokens);
        return true;
    }

    return false;
}

int tokenizeMultiChar(char **tokenStart, tokenList *tokens)
{
    char *canonicalStart = *tokenStart;
    while (isalnum(**tokenStart) || **tokenStart == '_')
    {
        *tokenStart += 1;
    }

    bool isOpcode = false;
    if ((*tokenStart - canonicalStart) == 1)
    {
        return tokenizeSingleChar(*canonicalStart, tokens);
    }
    else if ((*tokenStart - canonicalStart) == 3)
    {
        isOpcode = tokenizeOpcode(canonicalStart, tokens);
    }

    return (isOpcode) ? SUCCESS : tokenizeLabel(canonicalStart, (*tokenStart - canonicalStart), tokens);
}

int tokenizeDirective(char **tokenStart, tokenList *tokens)
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
            pushToken((token){BYTEDIRECTIVE, NULL, NULL}, tokens);
            return SUCCESS;
        }
        else if (!memcmp(canonicalStart, "word", 4))
        {
            pushToken((token){WORDDIRECTIVE, NULL, NULL}, tokens);
            return SUCCESS;
        }
    }
    return UNDEFINEDDIRECTIVE;
}

void readWhiteSpace(char **tokenStart)
{
    while (**tokenStart == ' ' || **tokenStart == '\t' || **tokenStart == '\v')
    {
        *tokenStart += 1;
    }
}

int tokenize(char **tokenStart, tokenList *tokens)
{
    readWhiteSpace(tokenStart);
 
    switch (**tokenStart)
    {
    case ':':
        pushToken((token){COLON, NULL, NULL}, tokens);
        *tokenStart += 1;
        return SUCCESS;
    case '#':
        pushToken((token){HASH, NULL, NULL}, tokens);
        *tokenStart += 1;
        return SUCCESS;
    case '(':
        pushToken((token){OPENPAREN, NULL, NULL}, tokens);
        *tokenStart += 1;
        return SUCCESS;
    case ')':
        pushToken((token){CLOSEPAREN, NULL, NULL}, tokens);
        *tokenStart += 1;
        return SUCCESS;
    case ',':
        pushToken((token){COMMA, NULL, NULL}, tokens);
        *tokenStart += 1;
        return SUCCESS;
    case '.':
        pushToken((token){PERIOD, NULL, NULL}, tokens);
        *tokenStart += 1;
        return tokenizeDirective(tokenStart, tokens);
    case '\n':
        return SUCCESS;
    case '\r':
        return SUCCESS;
    case '\0':
        return SUCCESS;
    case ';':
        return SUCCESS;
    case '$':
        pushToken((token){DOLLAR, NULL, NULL}, tokens);
        *tokenStart += 1;
        return tokenizeHex(tokenStart, tokens);
    case '%':
        pushToken((token){PERCENT, NULL, NULL}, tokens);
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

int tokenizeLine(char **lineStart, tokenList *tokens)
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

        int tokenizationReturn =tokenize(lineStart, tokens);
        if (tokenizationReturn != SUCCESS)
        {
            return tokenizationReturn;
        }
    }

    return SUCCESS;
}

void addErrorData(int lineNumber, char* lineStart, token* tokens, int numTokens)
{
    for(int i = 0; i <numTokens; i++)
    {
        errorData** newErr = &((tokens+i)->err);
        *newErr = malloc(sizeof(errorData));
        (*(newErr))->lineNumber = lineNumber;
        (*(newErr))->lineStart = lineStart;
    }
}

void tokenizeFile(char *content, tokenList *tokens)
{
    char *currentChar = content;
    int previousLen = tokens->length;

    char* lineStart = content;
    int lineNumber = 1;
    while (*currentChar != '\0')
    {
        int tokenizationReturn = tokenizeLine(&currentChar, tokens);

        if (tokenizationReturn != SUCCESS)
        {   
            errorData err = (errorData){.lineNumber = lineNumber, .lineStart = lineStart};
            printError(tokenizationReturn, &err);
            exit(-1);
        }

        if (previousLen < tokens->length)
        {
            addErrorData(lineNumber, lineStart, tokens->content + previousLen, tokens->length-previousLen);
            pushToken((token){NEWLINE, NULL, NULL}, tokens);
        }

        previousLen = tokens->length;
        currentChar++;

        lineStart = currentChar;
        lineNumber++;
    }


    pushToken((token){FILEEND, NULL, NULL}, tokens);
}
