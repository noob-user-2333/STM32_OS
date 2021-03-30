		IMPORT os_thread_current_ptr
		IMPORT task_switch_context
	AREA  ||.text||,CODE,READONLY
	PRESERVE8
	EXPORT os_thread_schedule
os_thread_schedule
	
;	MOV     r0, #0x10000000                         ; Load PENDSVSET bit
;    MOV     r1, #0xE000E000                         ; Load NVIC base
;    STR     r0, [r1, #0xD04]                        ; Set PENDSVBIT in ICSR
;重定位msp
	LDR  r0,=0xE000ED08
	LDR  r0,[r0]
	LDR  r0,[r0]
	MSR  msp,r0
	CPSIE I
	CPSIE F
    DSB                                             ; Complete all memory accesses
    ISB
	SVC 0
	NOP
	NOP
	EXPORT SVC_Handler
SVC_Handler	
	ldr	r3, =os_thread_current_ptr	
	ldr r1, [r3]			
	ldr r0, [r1,#12]			
	ldmia r0!, {r4-r11}		
	msr psp, r0				
	isb
	mov r0, #0
	msr	basepri, r0
	orr r14, #0xd
	bx r14
	
os_wait_here
	B os_wait_here

	EXPORT PendSV_Handler
PendSV_Handler
	MRS r0,psp
	isb
	LDR r3,=os_thread_current_ptr
	LDR r2,[r3]
	
	STMDB r0!,{r4-r11}
	STR   r0,[r2,#12]
	
	STMDB sp!,{r3,r14}
	MOV	  r0,#0xE0
	MSR   basepri,r0
	DSB
	ISB
	
	BL 	 task_switch_context
	MOV  r0,#0
	MSR  basepri,r0
	LDMIA sp!,{r3,r14}

	LDR r1,[r3]
	LDR r0,[r1,#12]
	LDMIA r0!,{r4-r11}
	MSR	 psp,r0
	ISB
	BX	 r14
	NOP
	ALIGN
;	LTORG
	END
		