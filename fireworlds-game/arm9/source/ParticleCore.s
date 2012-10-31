
	.section .itcm,"ax",%progbits

	.global particleRender
particleRender:

	@R0 = begin
	@R1 = end
	@R2 = scene ptr

	
	STMFD	SP!, {R4-R12, R14}
	MOV 	R9, R0
	MOV 	R10, R1
	
	ldr 	R11, =0x04000400
	ldr 	R7, =xParticleOffs
	ldr 	R8, =yParticleOffs
	ldr		R7, [R7]
	ldr		R8, [R8]
	
	@==== 	R6 Poly ID
	mov 	R6, #0

renderLoop:

	CMP 	R9, R10		@if end
	LDMEQFD	SP!, {R4-R12, PC}		@return

	LDRH 	R5, [R9, #0x20] @Life
	CMP 	R5, #0
	BEQ 	badParticle
	
	LDMIA	R9, {R0, R1, R2, R3, R4, R5}   @Load x, y, vx, vy, vx2, vy2
	ADD 	R2, R2, R4, ASR #4		@vx += vx2
	ADD		R3, R3, R5, ASR #4		@vy += vy2
	ADD		R0, R0, R2		@x += vx
	ADD		R1, R1, R3		@y += vy

	LDRB	R4, [R9, #0x1D]
	CMP 	R4, #4
	CMPNE	R4, #5
	BEQ noFriction2
	ADD		R4, R2, #8
	ADD		R5, R3, #8
	SUB		R2, R2, R4, ASR #4
	SUB		R3, R3, R5, ASR #4
  noFriction2: 
	STMIA	R9, {R0, R1, R2, R3} @store x, y, vx, vy


	@===

	
	ldrsh 	R2, [R9, #0x20]
	subs	R2, R2, #1		@life--
	movmi	R2, #0	
	strh	R2, [R9, #0x20]
	
	ldrb	R2, [R9, #0x1D]
	cmp 	R2, #2
	bne		fx3
	
	ldrb	R4, [R9, #0x18]
	ldrb	R5, [R9, #0x19]
	ldrb	R1, [R9, #0x1A]
	add 	R4, R4, #10
	cmp 	R4, #256
	movge	R4, #255
	subs	R5, R5, #10
	movmi	R5, #0
	subs	R1, R1, #5
	movmi	R1, #0
	strb	R4, [R9, #0x18]
	strb	R5, [R9, #0x19]
	strb	R1, [R9, #0x1A]
fx3:
	cmp R2, #3
	bne fx0
	ldrb	R4, [R9, #0x1B]
	sub		R4, R4, #1
	strb	R4, [R9, #0x1B]
	
fx0:	
	
	
	@=== Set texture
	ldrb 	R0, [R9, #0x1F]
	bl _Z10setTexturei
	
	@=== Set color
	ldrb 	R0, [R9, #0x18]
	ldrb 	R1, [R9, #0x19]
	ldrb 	R2, [R9, #0x1A]
	mov 	R0, R0, LSR #3
	mov 	R1, R1, LSR #3
	mov 	R2, R2, LSR #3
	ORR 	R0, R1, LSL #5
	ORR 	R0, R2, LSL #10
	str 	R0, [R11, #0x80]
	
	@=== R3: Calc alpha
	LDRH 	R5, [R9, #0x20] @Life

	ldrb 	R2, [R9, #0x1D] @fx
	cmp 	R2, #3
	moveq	R3, R5, LSR #6
	addeq	R3, R3, #2
	ldrneb 	R3, [R9, #0x1B] @alpha
	cmp 	R2, #5
	moveq	R3, R5, LSR #4
	addeq	R3, R3, #3
	
	@=== Set poly attr
	mov 	R0, #3<<6				@No culling
	ORR 	R0, R0, R3, LSL#16		@Alpha
	ORR		R0, R0, R6, LSL#24		@Poly ID
	str		R0, [R11, #0xA4]		@Store it.
	
	ldrb 	R2, [R9, #0x1D] @fx
	cmp 	R2, #1
	bne		normalParticle
	mov 	R0, R9
	mov		R1, R11
@	bl		_ZN8Particle10renderBlurV
	b 		badParticle
	
normalParticle:
	@=== Begin vtxs
	mov		R0, #1
	str		R0, [R11, #0x100]
	
	@==== R5 Particle Size
	ldrsb 	R2, [R9, #0x1C]
	cmp 	R2, #0
	mov 	R0, #0
	sublt 	R5, R0, R2, LSL #5
	mulge	R5, R2, R5

	@==== R2, R3 Upper left pos
	LDMIA	R9, {R2, R3}
	SUB		R2, R2, R5 		@x -= size
	SUB		R3, R3, R5		@y -= size

	SUBS	R2, R2, R7		@x -= xCam

	MOV		R1, R2			@R1 = abs(x)
	RSBMI   R1, R2, #0		
	CMP		R1, #384*32		@if(R1 > 192px)
	movge	R0, #1			@set life  to 0


	SUBs	R3, R3, R8		@Same for y
	MOV		R1, R3
	RSBMI   R1, R3, #0
	CMP		R1, #256*32
	movge	R0, #1

	cmp 	R0, #1
	moveq 	R0, #0
	streqh 	R0, [R9, #0x20]
	
	ADD		R5, R5, R5 @diameter, not radius!

	@==== Send VTX 1
	MOV		R0, #0
	STR		R0, [R11, #0x88]
	
	MOV		R1, #1<<16
	SUB		R1, #1
	AND		R2, R2, R1
	AND		R3, R3, R1
	ORR		R1, R2, R3, LSL #16
	str		R1, [R11, #0x8C]
	str		R0, [R11, #0x8C]

	ADD		R0, R0, #64<<4
	STR		R0, [R11, #0x88]
	ADD		R1, R1, R5
@	ORR		R1, R2, R3, LSL #16
	str		R1, [R11, #0x94]

	ADD		R0, R0, #64<<4<<16
	STR		R0, [R11, #0x88]
	ADD		R1, R1, R5, LSL #16
@	ORR		R1, R2, R3, LSL #16
	str		R1, [R11, #0x94]
	
	SUB		R0, R0, #64<<4
	STR		R0, [R11, #0x88]
	SUB		R1, R1, R5
@	ORR		R1, R2, R3, LSL #16
	str		R1, [R11, #0x94]
badParticle:	

	@====
	add		R6, R6, #1
	cmp		R6, #50
	moveq	R6, #0

	add		R9, R9, #0x24 	@Next particle.
	
	b renderLoop
	
	
.global getStackPointer

getStackPointer:
	mov R0,sp
	bx lr
	
