#include <errors.h>
#include <stdio.h>

void printToLineEnd(char *lineStart)
{
    char *current = lineStart;
    while (*current != '\n' && *current != '\r')
    {
        printf("%c", *current);
        current++;
    }
}

void printError(int errorId, errorData *data)
{
    switch (errorId)
    {
    case BADHEXLENGTH:
        printf("Hex number of unexpected length, hex numbers must be either 2 or 4 characters,");
        break;
    case BADBINARYLENGTH:
        printf("Binary number of unexpected length, binary numbers must be either 8 or 16 characters,");
        break;
    case NUMBEROUTOFRANGE:
        printf("Numerical constant out of range for unsigned 16 bit integer");
        break;
    case UNDEFINEDDIRECTIVE:
        printf("Use of an uknown directive");
        break;
    default:
        printf("error with code %i", errorId);
        break;
    }
    printf(" on line: %i\n", data->lineNumber);
    printToLineEnd(data->lineStart);
    printf("\n");
}
