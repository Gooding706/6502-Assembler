#pragma once
#ifndef PARSER
#define PARSER
#include <ast.h>
#include <tokens.h>

ast *parseTokenList(tokenList *tokens);

#endif