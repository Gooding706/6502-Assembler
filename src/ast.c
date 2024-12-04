#include <ast.h>

void pushU16(uint16_t u16, u16List* list)
{
    if(list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(address_t));
    }
    list->content[list->length] = (address_t){.tag = constant, .data.addressLiteral = u16};
    list->length++;
}
void pushU8(uint8_t u8, u8List* list)
{
    if(list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(uint8_t));
    }
    list->content[list->length] = u8;
    list->length++;
}

void pushBranch(astBranch branch, ast* list)
{
    if(list->capacity <= list->length)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, list->capacity * sizeof(astBranch));
    }
    list->content[list->length] = branch;
    list->length++;
}
