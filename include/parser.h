#pragma once
#ifndef PARSER
#define PARSER
#include <ast.h>
#include <tokens.h>

bool parseTokenList(tokenList *tokens, ast** out);


#endif
