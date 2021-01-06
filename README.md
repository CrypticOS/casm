
# CASM
The official assembler for CrypticOS.  

## Building
You can configure if you want to compile with the terminal  
or the X11 graphical emulator, by changing USE_WINDOW in header.h.  
Of course if you wish to compile in windows, you would turn this off, and leave  
out the `-lX11`.
Run:  
`cc gfx/gfx.c emulate.c assemble/lex.c assemble/assemble.c main.c -o casm -lX11`  

A brief build script is in build.sh. Modify to your liking.

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
```
prt <string/int/char/variable>
var <name> <value (optional)>
arr <array name> <length> <initializing value (optional)>
equ <first value (string/int/char/variable)> <second value> <label to jump to>
add <variable name> <char/int>
sub <variable name> <char/int>
got <variable name> *Moves the pointer to a variable. WKSP can be used to go to the workspace cell.
ret * returns from a function run
run <label name> * Runs a label like a function. Must use ret to go back.
```

## Design
In CASM, the "bottom" memory is managed. The "workspace cell" is the last free cell  
in the bottom memory. For example,  
```
3 1 5 0
      | Workspace, temporary values are stored here.
```
