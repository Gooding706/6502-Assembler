#pragma once
#ifndef ERRORS
#define ERRORS

#define SUCCESS 0

//Lexer specific error codes
#define BADHEXLENGTH 1
#define BADBINARYLENGTH 2
#define NUMBEROUTOFRANGE 3
#define UNDEFINEDDIRECTIVE 4
#define UNRECOGNIZEDLABELSYMBOL 35

//Parser specific error codes
#define MISSINGCOLON 5
#define MISSINGHEXBYTE 6
#define MISSINGBINBYTE 7
#define MISSINGBYTETYPE 8
#define MISSINGCOMMA 9
#define MISSINGHEXWORD 10
#define MISSINGBINWORD 11
#define MISSINGWORDTYPE 12
#define MISSINGDIRECTIVE 13
#define MISSINGLABEL 14
#define MISSINGREGISTERLABEL 15
#define MISSINGNEWLINE 16
#define MISSINGYREGISTER 17
#define MISSINGXREGISTER 18
#define UNABLETOINFERADDRESSINGMODE 19
#define INVALIDELINESTART 20
#define MISSINGCLOSEPAREN 34
//Addressing mode specific error codes
#define NOTACCUMULATORADDRESSABLE 21
#define NOTABSOLUTEADDRESSABLE 22
#define NOTABSOLUTEXADDRESSABLE 23
#define NOTABSOLUTEYADDRESSABLE 24
#define NOTIMMEDIATEADDRESSABLE 25
#define NOTIMPLIEDADDRESSABLE 26
#define NOTINDIRECTADDRESSABLE 27
#define NOTINDIRECTXADDRESSABLE 28
#define NOTINDIRECTYADDRESSABLE 29
#define NOTRELATIVEADDRESSABLE 30
#define NOTZEROPAGEADDRESSABLE 31
#define NOTZEROPAGEXADDRESSABLE 32
#define NOTZEROPAGEYADDRESSABLE 33
//Generator Errors
#define FATALGENERATORERROR 36
#define BRANCHOUTOFRANGE 37
#define UNKNOWNLABEL 38

typedef struct{
    char* lineStart;
    int lineNumber;
}errorData;

void printError(int errorId, errorData *data);

#endif