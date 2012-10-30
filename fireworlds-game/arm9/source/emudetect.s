.arch armv5te

.text
.arm
.align 2


.global isEmulator
isEmulator: @ based on the "emulators don't emulate instruction fetching" exploit
	ldr r0, .Lmov_r0_r0
	ldr r2, .Lmov_r0_0
	mov r1, pc @ r1 = two instructions ahead of the current instruction
	str r0, [r1]
	mov r0, #0
	str r2, [r1]
	bx lr

.Lmov_r0_r0: mov r0, r0
.Lmov_r0_0:  mov r0, #0
