#include <stdio.h>
#include <lexer.h>
#include <tokens.h>

void printTokens(tokenList *tokens)
{
    char* names[] = {"ADC","AND","ASL","BCC","BCS","BEQ","BIT","BMI","BNE","BPL","BRK","BVC","BVS","CLC","CLD","CLI","CLV","CMP","CPX","CPY","DEC","DEX","DEY","EOR","INC","INX","INY","JMP","JSR","LDA","LDX","LDY","LSR","NOP","ORA","PHA","PHP","PLA","PLP","ROL","ROR","RTI","RTS","SBC","SEC","SED","SEI","STA","STX","STY","TAX","TAY","TSX","TXA","TXS","TYA","HASH","DOLLAR","COLON","PERCENT","COMMA","NEWLINE","OPENPAREN","CLOSEPAREN","FILEEND","PERIOD","LABEL","BYTEDIRECTIVE","WORDDIRECTIVE","XREGISTER","YREGISTER","HEXNUMBER8","HEXNUMBER16","BINARYNUMBER8","BINARYNUMBER16","NUMBER8","NUMBER16"};
    for(int i = 0; i < tokens->length; i++)
    {
        printf("%s\n", (tokens->content[i].tokenId <= 76) ? names[tokens->content[i].tokenId] : "UNDEFINED");
    }

}

int main()
{
    tokenList tokens = (tokenList){.content = malloc(sizeof(token)), .capacity = 1, .length = 0};
    char *fileContents = loadFile("tests/test1.asm");

    tokenizeFile(fileContents, &tokens);
    printTokens(&tokens);

    freeTokenList(&tokens);
    free(fileContents);
    return 0;
}