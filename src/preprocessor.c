#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <errors.h>

typedef struct
{
    char *name;
    char *replace;
} definition;

typedef struct
{
    definition *content;
    int capacity;
    int length;
} defineList;

int inDefineList(char *str, defineList *list)
{
    for (int i = 0; i < list->length; i++)
    {
        // printf("%s, %s\n", str,  list->content[i].name);
        if (!strcmp(str, list->content[i].name))
        {
            return i;
        }
    }
    return -1;
}

void pushDefine(definition def, defineList *list)
{
    if (list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(definition));
    }
    list->content[list->length] = def;
    list->length++;
}

// replace substring
char *strReplace(char *substrStart, char *substrEnd, char *replacement, char **str)
{
    int segmentOneLength = (int)(substrStart - (*str));
    int segmentTwoLength = strlen(*str) - ((int)(substrEnd - (*str)));
    int replacementLength = strlen(replacement);

    int outLen = segmentOneLength + replacementLength + segmentTwoLength;

    char *out = malloc(outLen + 1);
    out[outLen] = '\0';

    memcpy(out, *str, segmentOneLength);
    memcpy(out + segmentOneLength, replacement, replacementLength);
    memcpy(out + segmentOneLength + replacementLength, substrEnd, segmentTwoLength);

    free(*str);
    *str = out;

    return out + segmentOneLength + replacementLength;
}

char *readSubstrToSpace(char **start)
{
    char *canonicalStart = *start;
    bool spaceHead = true;

    while ((!isspace(**start) || spaceHead) && **start != '\0')
    {
        if (!isspace(**start))
        {
            spaceHead = false;
        }
        else
        {
            canonicalStart++;
        }
        *start += 1;
    }
    int substrLen = (*start - canonicalStart);
    char *out = malloc(substrLen + 1);
    out[substrLen] = '\0';
    memcpy(out, canonicalStart, substrLen);

    return out;
}

char *readSubstrToIgnored(char **start)
{
    char *canonicalStart = *start;
    while ((!isspace(**start) && isalnum(**start)) || **start == '_')
    {
        *start += 1;
    }

    int substrLen = (*start - canonicalStart);

    char *out = malloc(substrLen + 1);
    out[substrLen] = '\0';
    memcpy(out, canonicalStart, substrLen);

    return out;
}

int getDefinition(char **start, definition *out)
{
    char *name = readSubstrToSpace(start);
    *start += 1;
    char *replace = readSubstrToSpace(start);
    if (**start != '\0')
    {
        *start += 1;
    }
    else
    {
        return BADDEFINITION;
    }

    *out = (definition){.name = name, .replace = replace};
    return SUCCESS;
}

bool readSearchSubstr(char **start, char *substr, int *newLines)
{
    char *substrMatch = substr;
    int lines = 0;

    while (**start != '\0' && *substrMatch != '\0')
    {
        if (**start == *substrMatch)
        {
            substrMatch++;
        }
        else
        {
            substrMatch = substr;
        }

        if (**start == '\n')
        {
            lines++;
        }
        *start += 1;
    }

    *newLines = lines;
    return (*substrMatch == '\0');
}

char *removeSubstr(char *substrStart, char *substrEnd, char **str)
{
    int segmentOneLength = (substrStart - *str);
    int segmentTwoLength = strlen(*str) - ((int)(substrEnd - (*str)));

    int outLen = segmentOneLength + segmentTwoLength;
    char *out = malloc(outLen + 1);
    out[outLen] = '\0';

    memcpy(out, *str, segmentOneLength);
    memcpy(out + segmentOneLength, substrEnd, segmentTwoLength);
    free(*str);
    *str = out;

    return out + segmentOneLength;
}

bool fillDefineList(char **text, defineList *list)
{
    char *reader = *text;

    int lineNumber = 1;
    int additionalLines = 0;

    while (readSearchSubstr(&reader, ".define ", &additionalLines))
    {
        lineNumber += additionalLines;
        char *defineStart = reader - strlen(".define ");

        definition def;
        int returnVal = getDefinition(&reader, &def);

        if (returnVal != SUCCESS)
        {
            errorData err = (errorData){.lineNumber = lineNumber+1, .lineStart = defineStart};
            printError(returnVal, &err);
            return false;
        }

        int defIndex = inDefineList(def.name, list);
        if (defIndex == -1)
        {
            pushDefine(def, list);
        }
        else
        {
            errorData err = (errorData){.lineNumber = lineNumber+1, .lineStart = defineStart};
            printError(REDEFINITIONOFLABEL, &err);
            return false;
        }

        reader = removeSubstr(defineStart, reader, text);
    }
    return true;
}

void freeDefinition(definition *d)
{
    free(d->name);
    free(d->replace);
}

void freeDefineList(defineList *list)
{
    for (int i = 0; i < list->length; i++)
    {
        freeDefinition(&list->content[i]);
    }
    free(list->content);
}

void replaceFromDefineList(char **text, defineList *list)
{

    char *reader = *text;
    while (*reader != '\0')
    {
        char *substr = readSubstrToIgnored(&reader);

        int defIndex = inDefineList(substr, list);

        if (defIndex != -1)
        {
            reader = strReplace(reader - strlen(substr), reader, list->content[defIndex].replace, text);
        }

        if (*reader != '\0')
        {
            reader += 1;
        }
        free(substr);
    }
}

bool preprocess(char **text)
{
    defineList defines = (defineList){.content = malloc(sizeof(definition)), .capacity = 1, .length = 0};

    if (!fillDefineList(text, &defines))
    {
        freeDefineList(&defines);
        return false;
    }

    replaceFromDefineList(text, &defines);

    freeDefineList(&defines);
    return true;
}
