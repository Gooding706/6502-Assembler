#pragma once
#ifndef GENERATOR
#define GENERATOR
#include <ast.h>

bool assembleParseTree(ast *tree, int programOffset, char **outputData, int *len);

#endif
