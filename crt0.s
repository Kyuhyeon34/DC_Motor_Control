.syntax unified
	.thumb
	.text

	/* ===== Interrupt Vector Table ===== */
	.word	0x20020000        /* 0 : Initial SP */
	.word	__start           /* 1 : Reset (기존 그대로) */
	.word	Default_Handler   /* 2 : NMI */
	.word	Default_Handler   /* 3 : HardFault */
	.word	Default_Handler   /* 4 : MemManage */
	.word	Default_Handler   /* 5 : BusFault */
	.word	Default_Handler   /* 6 : UsageFault */
	.word	0                 /* 7 : Reserved */
	.word	0                 /* 8 : Reserved */
	.word	0                 /* 9 : Reserved */
	.word	0                 /* 10: Reserved */
	.word	Default_Handler   /* 11: SVCall */
	.word	Default_Handler   /* 12: DebugMon */
	.word	0                 /* 13: Reserved */
	.word	Default_Handler   /* 14: PendSV */
	.word	Default_Handler   /* 15: SysTick */

	/* ===== External Interrupts (IRQ0 ~ IRQ40) ===== */
	.word	Default_Handler   /* IRQ0  */
	.word	Default_Handler   /* IRQ1  */
	.word	Default_Handler   /* IRQ2  */
	.word	Default_Handler   /* IRQ3  */
	.word	Default_Handler   /* IRQ4  */
	.word	Default_Handler   /* IRQ5  */
	.word	Default_Handler   /* IRQ6  */
	.word	Default_Handler   /* IRQ7  */
	.word	Default_Handler   /* IRQ8  */
	.word	Default_Handler   /* IRQ9  */
	.word	Default_Handler   /* IRQ10 */
	.word	Default_Handler   /* IRQ11 */
	.word	Default_Handler   /* IRQ12 */
	.word	Default_Handler   /* IRQ13 */
	.word	Default_Handler   /* IRQ14 */
	.word	Default_Handler   /* IRQ15 */
	.word	Default_Handler   /* IRQ16 */
	.word	Default_Handler   /* IRQ17 */
	.word	Default_Handler   /* IRQ18 */
	.word	Default_Handler   /* IRQ19 */
	.word	Default_Handler   /* IRQ20 */
	.word	Default_Handler   /* IRQ21 */
	.word	Default_Handler   /* IRQ22 */
	.word	Default_Handler   /* IRQ23 (EXTI9_5) */
	.word	Default_Handler   /* IRQ24 */
	.word	Default_Handler   /* IRQ25 */
	.word	Default_Handler   /* IRQ26 */
	.word	Default_Handler   /* IRQ27 */
	.word	TIM2_IRQHandler   /* IRQ28 (TIM2)   <- 실제 핸들러 */
	.word	Default_Handler   /* IRQ29 */
	.word	TIM4_IRQHandler   /* IRQ30 (TIM4)   <- 실제 핸들러 */
	.word	Default_Handler   /* IRQ31 */
	.word	Default_Handler   /* IRQ32 */
	.word	Default_Handler   /* IRQ33 */
	.word	Default_Handler   /* IRQ34 */
	.word	Default_Handler   /* IRQ35 */
	.word	Default_Handler   /* IRQ36 */
	.word	Default_Handler   /* IRQ37 */
	.word	USART2_IRQHandler /* IRQ38 (USART2) <- 실제 핸들러 */
	.word	Default_Handler   /* IRQ39 */
	.word	EXTI15_10_IRQHandler /* IRQ40 (EXTI15_10) <- 실제 핸들러 */

	.extern	TIM2_IRQHandler
	.extern	TIM4_IRQHandler
	.extern	USART2_IRQHandler
	.extern	EXTI15_10_IRQHandler

	.thumb_func
Default_Handler:
	B	.

	/* ===== 기존 __start 코드, 전혀 수정 안 함 ===== */
	.global	__start
  	.type 	__start, %function
__start:

	.equ GPIOA_LCKR,	0x4002001C @ Lock PA[14:13] for ST-LINK
	.equ RCC_AHB1ENR,  	0x40023830 @ PA : [0] = 1, PC : [2] = 1

	@ GPIOA, GPIOC clock on
	LDR   	r0, =RCC_AHB1ENR
	LDR   	r1, [r0]
	ORR	  	r1, r1, #(0x1<<2)|(0x1<<0)
	STR   	r1, [r0]

	@ Lock PA[14:13] pin configuration for ST-LINK
	LDR		r0, =GPIOA_LCKR
	LDR		r1, =(0x1<<16)|(0x3<<13)
	STR		r1, [r0]
	LDR		r1, =(0x0<<16)|(0x3<<13)
	STR		r1, [r0]
	LDR		r1, =(0x1<<16)|(0x3<<13)
	STR		r1, [r0]
	LDR		r1, [r0]

	@ CRT Startup Code

	.extern __RO_LIMIT__
	.extern __RW_BASE__
	.extern __ZI_BASE__
	.extern __ZI_LIMIT__

	LDR		r0, =__RO_LIMIT__
	LDR		r1, =__RW_BASE__
	LDR		r3, =__ZI_BASE__

	CMP		r0, r1
	BEQ		2f

1:
	CMP		r1, r3
	ITT		LO
	LDRLO	r2, [r0], #4
	STRLO	r2, [r1], #4
	BLO		1b

2:
	LDR		r1, =__ZI_LIMIT__
	MOV		r2, #0x0
3:
	CMP		r3, r1
	ITT		lo
	STRLO	r2, [r3], #4
	BLO		3b

	.extern Main

	BL		Main

	B		.
			
	.end
