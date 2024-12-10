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
        default:
            break;
        }
    }
}