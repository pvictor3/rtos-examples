
    ; os_asm.s
    ; A very simple RTOS with round robin scheduler
    ; vmph
    ; 07/05/2019
    ; 

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  ptRun            ; currently running thread
        EXPORT  OS_disableInterrupts
        EXPORT  OS_enableInterrupts
        EXPORT  startOS
        EXPORT  SysTick_Handler


OS_disableInterrupts
        CPSID   I
        BX      LR


OS_enableInterrupts
        CPSIE   I
        BX      LR

		IMPORT	scheduler
SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
    PUSH    {R4-R11}           ; 3) Save remaining regs r4-11
    LDR     R0, =ptRun         ; 4) R0=pointer to RunPt, old thread
    LDR     R1, [R0]           ;    R1 = RunPt
    STR     SP, [R1]           ; 5) Save SP into TCB
    ;LDR     R1, [R1,#4]       ; 6) R1 = RunPt->next
    ;STR     R1, [R0]          ;    RunPt = R1
	
	PUSH	{R0, LR}
	BL		scheduler			;ptRun = ptRun->next, new thread
	POP		{R0, LR}
	
	LDR		R1, [R0]		   ; 6) R1 = ptRun, new thread 
    LDR     SP, [R1]           ; 7) new thread SP; SP = RunPt->sp;
    POP     {R4-R11}           ; 8) restore regs r4-11
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restore R0-R3,R12,LR,PC,PSR

startOS
    LDR     R0, =ptRun         ; currently running thread
    LDR     R2, [R0]           ; R2 = value of RunPt
    LDR     SP, [R2]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11
    POP     {R0-R3}            ; restore regs r0-3
    POP     {R12}
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R1}               ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END