
CalculaRST: mov.w R15,R13 ; R13 é o registrador temporário utilizado nas mini-divisoes.
	    mov.w R15,R11 ; Valor no qual a divisão é por zero.
	    sub.w #0,R13  ; verifica se a divisão é por zero.
	    jz FimDiv     ; se dividir por zero, o resto é o próprio R15.
looping:    mov.w R13,R11 ; resto da divisão será em R11, antes de subtrair o ultimo valor.
	    sub.w R14,R13 ; R13 = R13 - R14 subtrai o numerador do denominador, para testar...
	    jn FimDiv     ; divisao termina quando a subtração der negativa.
	    jmp looping   ; FOR
FimDiv:     mov.w R11,R15 ; transmite o valor do resto da div para R15
	    ret	    