
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

	LDRB	R12, [R9, #0x1D] @FX
	CMP 	R12, #4
	CMPNE	R12, #5
	BEQ noFriction2
	ADD		R4, R2, #8
	ADD		R5, R3, #8
	SUB		R2, R2, R4, ASR #4
	SUB		R3, R3, R5, ASR #4
  noFriction2: 
	STMIA	R9, {R0, R1, R2, R3} @store x, y, vx, vy


	@=== R5 Life
	ldrsh 	R5, [R9, #0x20]
	subs	R5, R5, #1		@life--
	movmi	R5, #0	
	strh	R5, [R9, #0x20]
	
	@ === RED color effect
	cmp 	R12, #2
	bne		fx3

	LDRB	R0, [R9, #0x18]
	LDRB	R1, [R9, #0x19]
	SUBS	R0, R0, #10
	MOVMI	R0, #0
	SUBS	R1, R1, #5
	MOVMI	R1, #0
	STRB	R0, [R9, #0x18]
	STRB	R1, [R9, #0x19]

fx3:
	
	@=== Set color
	LDR 	R0, [R9, #0x18]  @0xAARRBBGG
	MOV		R1, R0, LSR#16+3
	AND		R1, R1, #31
	MOV		R2, R0, LSR#3
	AND		R2, R2, #31
	ORR		R1, R1, R2, LSL#5
	MOV		R2, R0, LSR#11
	AND		R2, R2, #31
	ORR		R1, R1, R2, LSL#10
	STR 	R1, [R11, #0x80]

	
	@=== R3: Calc alpha
	MOV		R3, R0, LSR#24 @ Alpha
	CMP		R12, #3
	moveq	R3, R5, LSR #6
	addeq	R3, R3, #2
	CMP 	R12, #5
	moveq	R3, R5, LSR #4
	addeq	R3, R3, #3
	
	@=== Set poly attr
	mov 	R0, #3<<6				@No culling
	ORR 	R0, R0, R3, LSL#16		@Alpha
	ORR		R0, R0, R6, LSL#24		@Poly ID
	str		R0, [R11, #0xA4]		@Store it.
	
	cmp 	R12, #1
	bne		normalParticle
	@=== Set texture
	ldrb 	R0, [R9, #0x1F]
	bl _Z10setTexturei
	mov 	R0, R9
	mov		R1, R11
@	bl		_ZN8Particle10renderBlurEv
	b 		badParticle
	
normalParticle:
	@=== Set texture
	ldrb 	R0, [R9, #0x1F]
	bl _Z10setTexturei

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

	@	R0 = 0, set previously.
	
	SUBS	R2, R2, R7		@x -= xCam
	MOV		R1, R2			@R1 = abs(x)
	RSBMI   R1, R2, #0
	MOV		R0, R1, LSR#13

	SUBs	R3, R3, R8		@Same for y
	MOV		R1, R3
	RSBMI   R1, R3, #0
	ORRS	R0, R0, R1, LSR#12
	
	MOVNE 	R0, #0
	STRNEH 	R0, [R9, #0x20] @Set life to 0
	
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
	SUBS	R6, R6, #1
	MOVMI	R6, #49

	add		R9, R9, #0x24 	@Next particle.
	
	b renderLoop
	
	
.global getStackPointer

getStackPointer:
	mov R0,sp
	bx lr
	
