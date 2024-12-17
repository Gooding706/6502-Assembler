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


int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("Incorrect instruction format\nExpected format: asm6502 infile outfile\n");
    }

    char *fileContents = loadFile(argv[1]);
    if (!preprocess(&fileContents))
    {
        free(fileContents);
        return -1;
    }

    tokenList tokens = (tokenList){.content = malloc(sizeof(token)), .capacity = 1, .length = 0};
    if (!tokenizeFile(fileContents, &tokens))
    {
        cleanupTokenization(&tokens, fileContents);
        return -1;
    }

    ast *tree;
    if (!parseTokenList(&tokens, &tree))
    {
        cleanupParsing(&tokens, fileContents, tree);
        return -1;
    }

    char *outContent;
    int len;
    if (!assembleParseTree(tree, 0x0600, &outContent, &len))
    {
        cleanupGeneration(&tokens, fileContents, tree, outContent);
        return -1;
    }

    FILE *f = fopen(argv[2], "w");
    fwrite(outContent, len, 1, f);
    fclose(f);

    cleanupGeneration(&tokens, fileContents, tree, outContent);
    return 0;
}
