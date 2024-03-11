CODE
f PROC
dec rdx
zxc:
xor ebx, ebx
xor rsi, rsi
mov eax,[r8]
shifter:
inc rsi
mov ebx, [r8+(rsi)*4]
dec rsi
mov [r8+(rsi)*4], ebx
inc rsi
cmp rsi, rdx
jl shifter
mov [r8+(rsi)*4],eax
loop zxc
ret
f ENDP
END
