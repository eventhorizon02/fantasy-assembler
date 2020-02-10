# fantasy-assembler

This Assembler is for educational purposes only, it is written for a fantasy non existing CPU.
This was a project I had to write for a C class last year.
This is a ‘TWO PASS ASSEMBLER’ which means:
It needs a second pass over the source code to resolve the addresses of labels that could not be known when first encountered in the first pass.
Of course by the end of the first pass all labels would have been encountered and assigned a value already, and then during the second pass the translation to binary code can be completed.
This assembler will take an assembly language source code or a few and spit out an object code file, externals and entries.
Our fake CPU has 8 registers r0 through r7, each is a 12 bit register.
There are only 16 available assembly commands:
mov, cpp, add, sub, not, clr, lea, inc, dec, jmp, bne, red, prn, jsr, rts, stop.
The assembler supports labels and directives, for example:
.data 7, -57, +17, 9
STR: .string “abcdef”
.entry // this implies that a label is declared in the current source file but is referenced in another source file, for example:
.entry HELLO
HELLO: add 1,@r1 #this line means that it is possible to reference the label HELLO form another file but is declared in the current file.
.extern // this means that the label in use is declared (with a .entry) in another source file, for example
.extern HELLO.
Using the assembler:
You have to invoke the assembler with command line arguments that includes a list of source files to be compiled (assembled).
the assembler will create an object file .obj for each source file entered.
In addition it will create an external file for each source file that has declarations of external labels. It will also create a .ent file for each source file that has .entry declarations.
The source files must have .as extensions.
For example: ‘assembler x y hello’ will invoke the assembler on the source files x.as, y.as, hello.as
It that case the assembler will out put x.ob, y.ob and hello.ob plus .ent and .ext for each source file that has externals or entry declarations.
The .ob files are encoded in Base64.
Structure of the .ob file:
The first line has two decimal numbers the first the total length of the instructions segment followed by the length of the data segment.
The following lines contain the memory content, each line contains the content of a single word in order starting from address 100, the word is encoded in base64.
A word is 12 bits long. each 6 bits is converted to the appropriate char according to the table defining base64. Therefore with the exception of the first line each line in the .ob file will have two base64 chars.
Ideas for expansions:
This assembler is missing the linker, you could write that if you wish.
My plans are to write a full Z80 or 6502 assembler when I have some free time.
As far as licensing goes:
You can do whatever you want with this code.
