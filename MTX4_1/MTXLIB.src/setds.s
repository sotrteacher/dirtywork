        .globl _setds
_setds:
	push   bp
	mov    bp,sp

	mov    ax,4[bp]
	mov    ds,ax

	pop    bp
        ret 
