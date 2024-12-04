#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

void tokenizeFile(char *content, tokenList *tokens);
char *loadFile(const char *path);