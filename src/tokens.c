#include <tokens.h>
#include <stdlib.h>

void pushToken(token t, tokenList* list)
{
    if(list->length >= list->capacity)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, sizeof(token) * list->capacity);
    }
    list->content[list->length] = t;
    list->length++;
}

void freeTokenList(tokenList* list)
{
    for(int i = 0; i < list->length; i++)
    {
        free(list->content[i].textContent);
    }
    free(list->content);
}
