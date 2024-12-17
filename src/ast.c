#include <ast.h>

void pushU16(uint16_t u16, u16List *list)
{
    if (list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(address_t));
    }
    list->content[list->length] = (address_t){.tag = constant, .data.addressLiteral = u16};
    list->length++;
}
void pushU8(uint8_t u8, u8List *list)
{
    if (list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(uint8_t));
    }
    list->content[list->length] = u8;
    list->length++;
}

void pushBranch(astBranch branch, ast *list)
{
    if (list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(astBranch));
    }
    list->content[list->length] = branch;
    list->length++;
}

void freeAddress(address_t a)
{
    if (a.tag == label)
    {
        free(a.data.labelText);
    }
}

void freeU16List(u16List *list)
{
    for (int i = 0; i < list->length; i++)
    {
        freeAddress(list->content[i]);
    }

    free(list->content);
}

void freeU8List(u8List *list)
{
    free(list->content);
}

void freeAST(ast *list)
{
    for (int i = 0; i < list->length; i++)
    {
        switch (list->content[i].branchType)
        {
        case directiveBytes:
            freeU8List(&list->content[i].data.byteDirective);
            break;
        case directiveWords:
            freeU16List(&list->content[i].data.wordDirective);
            break;
        case absolute:
            freeAddress(list->content[i].data.absoluteMode.address);
            break;
        case absoluteX:
            freeAddress(list->content[i].data.absoluteMode.address);
            break;
        case absoluteY:
            freeAddress(list->content[i].data.absoluteMode.address);
            break;
        case label:
            free(list->content[i].data.labelText);
            break;
        case indirect:
            freeAddress(list->content[i].data.indirectMode.address);
            break;
        case relative:
            freeAddress(list->content[i].data.relativeMode.address);
            break;
        default:
            //nothing
            break;
        }
        free(list->content[i].err);
    }
    
    free(list->content);
}
