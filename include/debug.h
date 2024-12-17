#pragma once
#ifndef DEBUG
#define DEBUG
#include <ast.h>
#include <tokens.h>
void printToken(unsigned short id);
void printTokens(tokenList *tokens);
void printU8List(u8List *list);
void printAddress(address_t a);
void printU16List(u16List *list);
void printAST(ast *a);
#endif
