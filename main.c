#include <stdio.h>

#include <lexer.h>
#include <tokens.h>
#include <parser.h>
#include <generator.h>
#include <preprocessor.h>
#include <debug.h>

void cleanupTokenization(tokenList *tokens, char *fileContents)
{
    freeTokenList(tokens);
    free(fileContents);
}

void cleanupParsing(tokenList *tokens, char *fileContents, ast *tree)
{
    freeTokenList(tokens);
    free(fileContents);
    freeAST(tree);
    free(tree);
}

void cleanupGeneration(tokenList *tokens, char *fileContents, ast *tree, char *outContent)
{
    freeAST(tree);
    free(tree);
    freeTokenList(tokens);
    free(fileContents);
    free(outContent);
}

int main()
{
    tokenList tokens = (tokenList){.content = malloc(sizeof(token)), .capacity = 1, .length = 0};
    char *fileContents = loadFile("tests/test7.asm");
    preprocess(&fileContents);

    if (!tokenizeFile(fileContents, &tokens))
    {
        cleanupTokenization(&tokens, fileContents);
    }

    ast *tree;
    if (!parseTokenList(&tokens, &tree))
    {
        cleanupParsing(&tokens, fileContents, tree);
    }

    char *outContent;
    int len;
    if (!assembleParseTree(tree, 0x0600, &outContent, &len))
    {
       cleanupGeneration(&tokens, fileContents, tree, outContent);
    }

    FILE *f = fopen("dump.bin", "w");
    fwrite(outContent, len, 1, f);
    fclose(f);

    cleanupGeneration(&tokens, fileContents, tree, outContent);
    return 0;
}
