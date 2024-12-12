#include <stdio.h>
#include <lexer.h>
#include <tokens.h>
#include <parser.h>
#include <generator.h>

void printToken(unsigned short id)
{
    char *names[] = {"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC", "CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP", "JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI", "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA", "HASH", "DOLLAR", "COLON", "PERCENT", "COMMA", "NEWLINE", "OPENPAREN", "CLOSEPAREN", "FILEEND", "PERIOD", "LABEL", "BYTEDIRECTIVE", "WORDDIRECTIVE", "XREGISTER", "YREGISTER", "HEXNUMBER8", "HEXNUMBER16", "BINARYNUMBER8", "BINARYNUMBER16", "NUMBER8", "NUMBER16"};
    printf("%s", (id <= 76) ? names[id] : "UNDEFINED");
}

void printTokens(tokenList *tokens)
{
    for (int i = 0; i < tokens->length; i++)
    {
        printToken(tokens->content[i].tokenId);
        printf("\n");
    }
}

void printU8List(u8List *list)
{
    for (int i = 0; i < list->length; i++)
    {
        printf("%i,", (int)list->content[i]);
    }
}

void printAddress(address_t a)
{
    if (a.tag == constant)
    {
        printf("%i", a.data.addressLiteral);
    }
    else
    {
        printf("'%s'", a.data.labelText);
    }
}

void printU16List(u16List *list)
{
    for (int i = 0; i < list->length; i++)
    {
        printAddress(list->content[i]);
        printf(",");
    }
}

void printAST(ast *a)
{
    for (int i = 0; i < a->length; i++)
    {
        printf("%i ", a->content[i].branchType);
        switch (a->content[i].branchType)
        {
        case directiveBytes:
            printU8List(&a->content[i].data.byteDirective);
            break;
        case label:
            printf("'%s'", a->content[i].data.labelText);
            break;
        case directiveWords:
            printU16List(&a->content[i].data.wordDirective);
            break;
        case accumulator:
            printToken(a->content[i].data.opcodeOnly);
            break;
        case implied:
            printToken(a->content[i].data.opcodeOnly);
            break;
        case immediate:
            printToken(a->content[i].data.immediateMode.opcode);
            printf("  #%i", (int)a->content[i].data.immediateMode.value);
            break;
        case absolute:
            printToken(a->content[i].data.absoluteMode.opcode);
            printf("  ");
            printAddress(a->content[i].data.absoluteMode.address);
            break;
        case absoluteX:
            printToken(a->content[i].data.absoluteMode.opcode);
            printf("  ");
            printAddress(a->content[i].data.absoluteMode.address);
            printf("  ,x");
            break;
        case absoluteY:
            printToken(a->content[i].data.absoluteMode.opcode);
            printf("  ");
            printAddress(a->content[i].data.absoluteMode.address);
            printf("  ,y");
            break;
        case relative:
            printToken(a->content[i].data.relativeMode.opcode);
            printf("  ");
            printAddress(a->content[i].data.relativeMode.address);
            break;
        case indirect:
            printToken(a->content[i].data.indirectMode.opcode);
            printf("  (");
            printAddress(a->content[i].data.indirectMode.address);
            printf(")");
            break;
        case indirectX:
            printToken(a->content[i].data.indirectRegisterMode.opcode);
            printf("  (");
            printf("%i", (int)a->content[i].data.indirectRegisterMode.value);
            printf(",x)");
            break;
        case indirectY:
            printToken(a->content[i].data.indirectRegisterMode.opcode);
            printf("  (");
            printf("%i", (int)a->content[i].data.indirectRegisterMode.value);
            printf("),y");
            break;
        case zeroPage:
            printToken(a->content[i].data.zeroPageMode.opcode);
            printf("  %i", (int)a->content[i].data.zeroPageMode.value);
            break;
        case zeroPageX:
            printToken(a->content[i].data.zeroPageMode.opcode);
            printf("  %i,x", (int)a->content[i].data.zeroPageMode.value);
            break;
        case zeroPageY:
            printToken(a->content[i].data.zeroPageMode.opcode);
            printf("  %i,y", (int)a->content[i].data.zeroPageMode.value);
            break;
        default:
            break;
        }
        printf("\n");
    }
}

int main()
{
    tokenList tokens = (tokenList){.content = malloc(sizeof(token)), .capacity = 1, .length = 0};
    char *fileContents = loadFile("tests/test3.asm");
    tokenizeFile(fileContents, &tokens);


    ast *tree = parseTokenList(&tokens);
    printAST(tree);

    char* outContent;
    int len = assembleParseTree(tree, 0x8000, &outContent);
    FILE *f = fopen("dump.bin", "w");
    fwrite(outContent, len, 1, f);
    printf("%i\n", len);
    fclose(f);

    freeAST(tree);
    free(tree);

    freeTokenList(&tokens);
    free(fileContents);
    free(outContent);
    return 0;
}
