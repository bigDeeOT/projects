; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

NUM		EQU		4
DIV		EQU		0

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	PRESERVE8

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB



;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
		PUSH {R1-R4,LR}
		MOV R5,#0
		MOV R4,#10
		MOV R7,#1
		PUSH {R0}
		CMP R0,R4
		BLO SINGLE
		
		;counting digits
DIGITS	UDIV R0,R0,R4
		ADD R5,R5,#1			; R5 holds the number of digits
		ADDS R0,R0,#0
		BNE DIGITS
	
		SUBS R5,R5,#1
COUNT1	MUL R7,R7,R4
		SUBS R5,R5,#1
		BNE COUNT1
		PUSH {R7}
		
STARTY	LDR R5,[SP,#NUM]
		LDR R6,[SP,#DIV]
		UDIV R0,R5,R6
		ADD R0,R0,#0x30
		BL ST7735_OutChar
		
		;R0 <- NUM % DIV -- these next 3 lines puts the remainder in R0
		UDIV R0,R5,R6
		MUL R0,R6,R0
		SUBS R0,R5,R0
		
		STR R0,[SP,#NUM]
		UDIV R6,R6,R4
		STR R6,[SP,#DIV]
		ADDS R6,R6,#0
		BNE STARTY
		B	DONE1
		
SINGLE 	ADD R0,R0,#0x30
		BL ST7735_OutChar
		ADD SP, SP, #4
		POP {R1-R4,LR}
		BX	LR
		
DONE1	ADD SP, SP, #8
		POP {R1-R4,LR}
		BX  LR

;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
		PUSH {R1-R4,LR}
		MOV R5,#10000
		MOV R4,#10
		PUSH {R0}
		CMP R0,R5
		BHS TEN
		
		MOV R3, #1000
		PUSH {R3}
		
		LDR R5,[SP,#NUM]
		LDR R6,[SP,#DIV]
		UDIV R0,R5,R6
		ADD R0,R0,#0x30
		BL ST7735_OutChar
		MOV R0, #0x2E
		BL ST7735_OutChar
		
		;R0 <- NUM % DIV -- these next 3 lines puts the remainder in R0
		UDIV R0,R5,R6
		MUL R0,R6,R0
		SUBS R0,R5,R0
		
		STR R0,[SP,#NUM]
		UDIV R6,R6,R4
		STR R6,[SP,#DIV]
		ADDS R6,R6,#0
		
STARTY2	LDR R5,[SP,#NUM]
		LDR R6,[SP,#DIV]
		UDIV R0,R5,R6
		ADD R0,R0,#0x30
		BL ST7735_OutChar

		UDIV R0,R5,R6
		MUL R0,R6,R0
		SUBS R0,R5,R0
		
		STR R0,[SP,#NUM]
		UDIV R6,R6,R4
		STR R6,[SP,#DIV]
		ADDS R6,R6,#0
		BNE STARTY2
		B	DONE2
		
TEN 	MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2E
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		ADD SP, SP, #4
		POP {R1-R4,LR}
		BX	LR
		
DONE2	ADD SP, SP, #8
		POP {R1-R4,LR}
		BX  LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file