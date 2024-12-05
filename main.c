#include <stdio.h>
#include <lexer.h>
#include <tokens.h>
#include <parser.h>

void printTokens(tokenList *tokens)
{
    char *names[] = {"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC", "CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP", "JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI", "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA", "HASH", "DOLLAR", "COLON", "PERCENT", "COMMA", "NEWLINE", "OPENPAREN", "CLOSEPAREN", "FILEEND", "PERIOD", "LABEL", "BYTEDIRECTIVE", "WORDDIRECTIVE", "XREGISTER", "YREGISTER", "HEXNUMBER8", "HEXNUMBER16", "BINARYNUMBER8", "BINARYNUMBER16", "NUMBER8", "NUMBER16"};
    for (int i = 0; i < tokens->length; i++)
    {
        printf("%s\n", (tokens->content[i].tokenId <= 76) ? names[tokens->content[i].tokenId] : "UNDEFINED");
    }
}

void printU8List(u8List *list)
{
    for (int i = 0; i < list->length; i++)
    {
        printf("%i,", (int)list->content[i]);
    }
}

void printU16List(u16List *list)
{
    for (int i = 0; i < list->length; i++)
    {
        if (list->content[i].tag == constant)
        {
            printf("%i,", (int)list->content[i].data.addressLiteral);
        }else
        {
            printf("'%s',", list->content[i].data.labelText);
        }
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
        default:
            break;
        }
        printf("\n");
    }
}

// TODO: recognize tab
int main()
{
    tokenList tokens = (tokenList){.content = malloc(sizeof(token)), .capacity = 1, .length = 0};
    char *fileContents = loadFile("tests/test2.asm");
    tokenizeFile(fileContents, &tokens);
    printTokens(&tokens);

    ast *tree = parseTokenList(&tokens);
    printAST(tree);

    freeTokenList(&tokens);
    free(fileContents);
    return 0;
}