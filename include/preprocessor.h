#pragma once
#ifndef PREPROCESSOR
#define PREPROCESSOR

typedef struct
{
    char *name;
    char *replace;
} definition;

typedef struct
{
    definition *content;
    int capacity;
    int length;
} defineList;

void preprocess(char** text);
#endif
