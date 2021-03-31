		IMPORT os_thread_current_ptr
		IMPORT task_switch_context
		IMPORT os_state
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
	MOV     r0, #0x10000000                         ; Load PENDSVSET bit
    MOV     r1, #0xE000E000                         ; Load NVIC base
    STR     r0, [r1, #0xD04]                        ; Set PENDSVBIT in ICSR
;	SVC 0
	DSB
	ISB
	NOP
	NOP
os_wait_here
	NOP
	NOP
	NOP
	B os_wait_here
	
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
	B os_wait_here

	EXPORT PendSV_Handler
PendSV_Handler
	LDR r0,=os_state
	LDR r1,[r0]
	CBZ	r1,os_first_start

	MRS r0,psp
	ISB
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
	NOP
	
os_first_start
	ldr	r3, =os_thread_current_ptr	
	ldr r1, [r3]			
	ldr r0, [r1,#12]			
	ldmia r0!, {r4-r11}		
	msr psp, r0				
	isb
	mov r0, #0
	msr	basepri, r0
	MOV	r2,#1
	LDR r1,=os_state
	STR	r2,[r1]
	orr r14, #0xd
	bx r14
	B os_wait_here
	
	ALIGN
;	LTORG
	END
		