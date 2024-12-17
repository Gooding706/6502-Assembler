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

bool preprocess(char** text);
#endif
