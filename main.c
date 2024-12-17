#include <stdio.h>
#include <lexer.h>
#include <tokens.h>
#include <parser.h>
#include <generator.h>
#include <preprocessor.h>
#include <debug.h>

int main()
{
    tokenList tokens = (tokenList){.content = malloc(sizeof(token)), .capacity = 1, .length = 0};
    char *fileContents = loadFile("tests/test7.asm");
    preprocess(&fileContents);

    if (!tokenizeFile(fileContents, &tokens))
    {
        freeTokenList(&tokens);
        free(fileContents);
    }

    ast *tree;
    if (!parseTokenList(&tokens, &tree))
    {
        freeTokenList(&tokens);
        free(fileContents);
        freeAST(tree);
        free(tree);
    }

    char *outContent;
    int len;
    if (!assembleParseTree(tree, 0x0600, &outContent, &len))
    {
        freeAST(tree);
        free(tree);
        freeTokenList(&tokens);
        free(fileContents);
        free(outContent);
    }

    FILE *f = fopen("dump.bin", "w");
    fwrite(outContent, len, 1, f);
    fclose(f);

    freeAST(tree);
    free(tree);

    freeTokenList(&tokens);
    free(fileContents);
    free(outContent);
    return 0;
}
