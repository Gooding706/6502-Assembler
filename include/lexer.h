#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

bool tokenizeFile(char *content, tokenList *tokens);
char *loadFile(const char *path);
