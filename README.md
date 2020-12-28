# CASM
The official assembler for CrypticOS.  

# File structure
`www` Is an assembler written in Javascript. It can be used online.  
`cli` Is the C version. It has more features and is obviously faster.  

## Syntax
CASM is based on a stripped down version of the NASM Syntax.  
```
var a 'A'
top:
prt a
prt "Hello, World."
jmp top
```

```
; In order to compare two values:
var  a 'A'
equ a 'B' mylbl ; this will not jump
mylbl:

; These instructions are fairly straightforward.
sub a 1
add a 1
add a '0'
set a 'Z'
```
