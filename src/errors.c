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
    printf("["
           "\033[1;91m"
           "ERROR"
           "\033[0m"
           "]: ");
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
    case NOTACCUMULATORADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Accumulator addressing");
        break;
    case NOTABSOLUTEADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Absolute addressing");
        break;
    case NOTABSOLUTEXADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Absolute X addressing");
        break;
    case NOTABSOLUTEYADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Absolute Y addressing");
        break;
    case NOTIMMEDIATEADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Immediate addressing");
        break;
    case NOTIMPLIEDADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Implied addressing");
        break;
    case NOTINDIRECTADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Indirect addressing");
        break;
    case NOTINDIRECTXADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Indirect Y addressing");
        break;
    case NOTINDIRECTYADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Indirect X addressing");
        break;
    case NOTRELATIVEADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Relative addressing");
        break;
    case NOTZEROPAGEADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Zero Page addressing");
        break;
    case NOTZEROPAGEXADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Zero Page X addressing");
        break;
    case NOTZEROPAGEYADDRESSABLE:
        printf("Attempting to use an incompatible instruction with Zero Page Y addressing");
        break;
    case FATALGENERATORERROR:
        printf("Fatal error during generation, this means there is a bug in the assembler's code");
        break;
    case BRANCHOUTOFRANGE:
        printf("Branch instructions may only branch within the range -127 to 127");
        break;
    case UNKNOWNLABEL:
        printf("Unknown label");
        break;
    case REDEFINITIONOFLABEL:
        printf("Redefinition of an already defined label");
        break;
    case BADDEFINITION:
        printf("Definition defined incorrectly");
        break;
    default:
        printf("error with code %i", errorId);
        break;
    }
    printf(" on line: %i\n", data->lineNumber);
    printf("%i| ", data->lineNumber);
    printToLineEnd(data->lineStart);
    printf("\n");
}
