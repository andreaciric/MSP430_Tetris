;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file


;-------------------------------------------------------------------------------
			.ref	rotation


;-------------------------------------------------------------------------------
			.sect ".text:_isr"

PORT1_ISR	PUSHM.A	#1, R15

			CLR.B	&P1IFG;
			BIT.B	#BIT4, &P1IN
			JNE		exit

			;rotation = (rotation + 1) % 4;
			MOV.W	#1, R15
			ADD.B	&rotation, R15
			AND.B	#0x0003, R15
			MOV.B	R15, &rotation

exit		POPM.A	#1, R15
			reti


;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------

            .sect   ".int47"                ; MSP430 PORT1_VECTOR
            .short  PORT1_ISR
