
# CASM
The official assembler for CrypticOS.  

## Building
You can configure if you want to compile with the terminal  
or the X11 graphical emulator, by changing USE_WINDOW in header.h.  
Of course if you wish to compile in windows, you would turn this off, and leave  
out the `-lX11`.
Run:
`cc gfx/gfx.c emulate.c assemble/lex.c assemble/assemble.c main.c -o casm -lX11`

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

## Design
In CASM, the "bottom" memory is managed.
