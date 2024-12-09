#include <tokens.h>
#include <stdlib.h>
#include <stdarg.h>

void pushToken(token t, tokenList *list)
{
    if (list->length >= list->capacity)
    {
        list->capacity *= 2;
        list->content = realloc(list->content, sizeof(token) * list->capacity);
    }
    list->content[list->length] = t;
    list->length++;
}

void freeTokenList(tokenList *list)
{
    for (int i = 0; i < list->length; i++)
    {
        free(list->content[i].textContent);
    }
    free(list->content);
}

bool isTokenOfType(int id, int count, ...)
{
     va_list args;
     va_start(args, count);
    for (int i = 0; i < count; i++)
    {
        if(id == va_arg(args, int))
        {
            return true;
        }
    }
    return false;
}
