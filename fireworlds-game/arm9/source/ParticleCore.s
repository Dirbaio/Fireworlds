

	.section .itcm,"ax",%progbits

	.global particleMove
particleMove:

	@r0 = begin
	@r1 = end
	@r2 = bufferbegin
	@r3 = bufferend
	STMFD SP!, {r4-r12}

moveLoop:
	cmp 	r0, r1		@if end
	ldmeqfd	SP!, {r4-r12}
	bxeq	lr			@return

	ldmia	R0, {r2, r3, r4, r5, r6, r7}   @Load x, y, vx, vy, vx2, vy2
	ADD 	r4, r4, r6, ASR #4		@vx += vx2
	ADD		r5, r5, r7, ASR #4		@vy += vy2
	ADD		r2, r2, r4		@x += vx
	ADD		r3, r3, r5		@y += vy
	LDRB	R6, [R0, #0x1D]
	CMP 	R6, #4
	CMPNE	R6, #5
	BEQ noFriction
	ADD		r6, r4, #8
	ADD		r7, r5, #8
	SUB		r4, r4, r6, ASR #4
	SUB		r5, r5, r7, ASR #4

  noFriction:
	stmia	R0!, {r2, r3, r4, r5} @store x, y, vx, vy
	
	@Now r0 = this+0x10
	
	ldrsh 	r2, [r0, #0x10]
	subs	r2, r2, #1		@life--
	movmi	r2, #0	
	strh	r2, [r0, #0x10]
	
	ldrb	r2, [r0, #0xD]
	cmp 	r2, #2
	bne		fx3
	
	ldrb	r4, [r0, #0x8]
	ldrb	r5, [r0, #0x9]
	ldrb	r6, [r0, #0xA]
	add 	r4, r4, #3
	cmp 	r4, #256
	movge	r4, #255
	subs	r5, r5, #4
	movmi	r5, #0
	subs	r6, r6, #1
	movmi	r6, #0
	strb	r4, [r0, #0x8]
	strb	r5, [r0, #0x9]
	strb	r6, [r0, #0xA]
fx3:
	cmp r2, #3
	bne fx0
	ldrb	r4, [r0, #0xB]
	sub		r4, r4, #1
	strb	r4, [r0, #0xB]
	
fx0:
	
	add		r0, r0, #0x14 	@Next particle.
	
	b moveLoop
	
	
	


	.section .itcm,"ax",%progbits

	.global particleRender
particleRender:

	@r0 = begin
	@r1 = end
	@r2 = scene ptr

	
	STMFD	SP!, {r4-r12, r14}
	MOV 	r9, r0
	MOV 	r10, r1
	MOV		r11, r2 @R11 = scene ptr
	
	ldr 	r4, =0x04000400
	ldr 	r7, =xParticleOffs
	ldr 	r8, =yParticleOffs
	ldr		r7, [r7]
	ldr		r8, [r8]
	
	@==== R5 Poly ID
	mov r5, #0
renderLoop:

	cmp 	r9, r10		@if end
	ldmeqfd	SP!, {r4-r12, r14}
	bxeq	lr			@return

	ldrh 	r6, [r9, #0x20] @Life
	cmp r6, #0
	beq badParticle
	
	@=== Set texture
	ldrb 	r0, [r9, #0x1F]
	bl _Z10setTexturei
	
	@=== Set color
	ldrb 	r0, [r9, #0x18]
	ldrb 	r1, [r9, #0x19]
	ldrb 	r2, [r9, #0x1A]
	mov r0, r0, LSR #3
	mov r1, r1, LSR #3
	mov r2, r2, LSR #3
	ORR r0, r1, LSL #5
	ORR r0, r2, LSL #10
	str r0, [r4, #0x80]
	
	@=== r3: Calc alpha
	ldrb 	r2, [r9, #0x1D] @fx
	cmp 	r2, #3
	moveq	r3, r6, LSR #6
	addeq	r3, r3, #2
	ldrneb 	r3, [r9, #0x1B] @alpha
	cmp 	r2, #5
	moveq	r3, r6, LSR #4
	addeq	r3, r3, #3
	
	@=== Set poly attr
	mov 	r0, #3<<6			@No culling
	ORR 	r0, r0, r3, LSL#16	@Alpha
	ORR		r0, r0, r5, LSL#24	@Poly ID
	str		r0, [r4, #0xA4]		@Store it.
	
	ldrb 	r2, [r9, #0x1D] @fx
	cmp 	r2, #1
	bne		normalParticle
	mov 	r0, r9
	mov		r1, r11
	bl		_ZN8Particle10renderBlurEP5Scene
	b 		badParticle
	
normalParticle:
	@=== Begin vtxs
	mov		r0, #1
	str		r0, [r4, #0x100]
	
	@==== R6 Particle Size
	ldrsb 	r2, [r9, #0x1C]
	cmp 	r2, #0
	mov 	r0, #0
	sublt 	r6, r0, r2, LSL #5
	mulge	r6, r2, r6

	@==== R2, R3 Upper left pos
	LDMIA	r9, {r2, r3}
	SUB		r2, r2, r6 	@x -= size
	SUB		r3, r3, r6		@y -= size

	SUBs	r2, r2, r7		@x -= xCam

	MOV		r1, r2			@r1 = abs(x)
	RSBMI   r1, r2, #0		
	CMP		r1, #384*32		@if(r1 > 192px)
	movge	r0, #1			@set life  to 0


	SUBs	r3, r3, r8		@Same for y
	MOV		r1, r3
	RSBMI   r1, r3, #0
	CMP		r1, #256*32
	movge	r0, #1

	cmp 	r0, #1
	moveq 	r0, #0
	streqh 	r0, [r9, #0x20]
	
	ADD		r6, r6, r6 @diameter, not radius!
	
	@==== Send VTX 1
	MOV		r0, #0
	STR		r0, [r4, #0x88]
	
	MOV		r1, #1<<16
	SUB		r1, #1
	AND		r2, r2, r1
	AND		r3, r3, r1
	ORR		r1, r2, r3, LSL #16
	str		r1, [r4, #0x8C]
	str		r0, [r4, #0x8C]

	ADD		r0, r0, #64<<4
	STR		r0, [r4, #0x88]
	ADD		r1, r1, r6
@	ORR		r1, r2, r3, LSL #16
	str		r1, [r4, #0x94]

	ADD		r0, r0, #64<<4<<16
	STR		r0, [r4, #0x88]
	ADD		r1, r1, r6, LSL #16
@	ORR		r1, r2, r3, LSL #16
	str		r1, [r4, #0x94]
	
	SUB		r0, r0, #64<<4
	STR		r0, [r4, #0x88]
	SUB		r1, r1, r6
@	ORR		r1, r2, r3, LSL #16
	str		r1, [r4, #0x94]
badParticle:	

	@====
	add		r5, r5, #1
	cmp		r5, #50
	moveq	r5, #0

	add		r9, r9, #0x24 	@Next particle.
	
	b renderLoop
	
	
.global getStackPointer

getStackPointer:
	mov r0,sp
	bx lr
	