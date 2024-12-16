## WARNING : This project is work in progress

## A 6502 Assembler
this is a simple low level assembler for the 6502 instruction set. This project was primarily for fun and for learning rather than to be used as a legitimate tool for widespread use.

## Assembler Features

.byte directive will add the listed bytes to the source file.

.word directive will add the listed 16bit words to the source file.

.define will create a definition to be used by the preprocessor to carry out find and replace operations.

Labels reference a specific place in code and are useful in branch instructions.

Comments are prefixed with a ';'.

## What am I still working on?
Error handling is pretty abysmal at the moment and needs quite a bit of attention.

I would like to add a '.include' preprocessor directive for multi-file projects.

The use of this as a console application has yet to be implemented.

Whatever else I feel an urge to include.