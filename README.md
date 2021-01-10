
# CASM
The official assembler for CrypticOS.  

## Building
You can configure if you want to compile with the terminal  
or the X11 graphical emulator, by changing USE_WINDOW in header.h.  
Of course if you wish to compile in windows, you would turn this off, and leave  
out the `-lX11`.
Run:  
`cc gfx/gfx.c emulate.c assemble/lex.c assemble/assemble.c main.c -o casm -lX11`  

A brief build script is in build.sh. Modify it to your liking.

## Syntax
CASM is based on a stripped down version of the NASM Syntax.  
```
; Infinite loop
var a 'A'
top:
prt a
prt "Hello, World."
jmp top

; In order to compare two values:
var a 'A'
equ a 'B' mylbl ; this will not jump
mylbl:

; These instructions are fairly straightforward.
sub a 1
add a 1
add a '0'
set a 'Z'
```

## Instructions
| Instruction | Arguments | Note |
|--|--|--|
| jmp | [label] |
| got | [variable name] | Moves the pointer to a variable. WKSP can be used to go to the workspace cell. |
| ret | | Returns from run instruction |
| inl | [string] | Paste CINS instruction in output |
| prt | [string/int/char/variable] |
| equ | [first value (string/int/char/variable)] [second value] [label to jump to] |
| set | [variable name] [value] |
| var | [variable name] [value] |
| arr | [array name] [length] [initializing value (optional)] |
| add | [variable name] [char/int] |
| sub | [variable name] [char/int] |
| run | [label name] | Runs a label like a function. Must use ret to go back. |
| def | [defined name] [value] | Works the same as C #define |
| inc | [file name, string] | Include a file, basically a copy/paste. |
| fre | [variable name] | Free a variable from assembler's memory. (experimental, may be removed) |

Etc:
- WKSP is a built-in variable. It points to the memory cell  
directly after the last variable defined.
