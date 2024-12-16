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
    case MISSINGCOLON:
        printf("Missing colon following label definition");
        break;
    case MISSINGHEXBYTE:
        printf("Missing hex valued byte");
        break;
    case MISSINGBINBYTE:
        printf("Missing binary valued byte");
        break;
    case MISSINGBYTETYPE:
        printf("Missing byte type");
        break;
    case MISSINGCOMMA:
        printf("Missing comma");
        break;
    case MISSINGHEXWORD:
        printf("Missing hex valued word");
        break;
    case MISSINGBINWORD:
        printf("Missing binary valued word");
        break;
    case MISSINGWORDTYPE:
        printf("Missing word type");
        break;
    case MISSINGDIRECTIVE:
        printf("Missing directive following .");
        break;
    case MISSINGLABEL:
        printf("Missing label");
        break;
    case MISSINGREGISTERLABEL:
        printf("Missing label reffering to the X or Y Register");
        break;
    case MISSINGNEWLINE:
        printf("Missing new line");
        break;
    case MISSINGXREGISTER:
        printf("Missing label reffering to the X Register");
        break;
    case MISSINGYREGISTER:
        printf("Missing label reffering to the Y Register");
        break;
    case UNABLETOINFERADDRESSINGMODE:
        printf("Instruction format is not valid for any of our supported addressing modes");
        break;
    case INVALIDELINESTART:
        printf("Invalid token at line start");
        break;
    case MISSINGCLOSEPAREN:
        printf("Missing closing parenthesis");
        break;
    case UNRECOGNIZEDLABELSYMBOL:
        printf("Label names are restricted to aplhanumeric characters and '_', use of a symbol that is unsupported by this assembler");
        break;
    default:
        printf("error with code %i", errorId);
        break;
    }
    printf(" on line: %i\n", data->lineNumber);
    printToLineEnd(data->lineStart);
    printf("\n");
}
