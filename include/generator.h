#pragma once
#ifndef GENERATOR
#define GENERATOR
#include <ast.h>

int assembleParseTree(ast *tree, int programOffset, char **outputData);
#endif
