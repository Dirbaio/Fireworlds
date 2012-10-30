#ifndef _FIFOCOMMANDS_H
#define _FIFOCOMMANDS_H

// define libnds types in terms of stdint
#include <stdint.h>

//! 8 bit unsigned integer.
typedef uint8_t		uint8;
//! 16 bit unsigned integer.
typedef uint16_t	uint16;
//! 32 bit unsigned integer.
typedef uint32_t	uint32;
//! 64 bit unsigned integer.
typedef uint64_t	uint64;

//! 8 bit signed integer.
typedef int8_t		int8;
//! 16 bit signed integer.
typedef int16_t		int16;
//! 32 bit signed integer.
typedef int32_t		int32;
//! 64 bit signed integer.
typedef int64_t		int64;

//! 32 bit signed floating point number.
typedef float		float32;
//! 64 bit signed floating point number.
typedef double		float64;

//! 8 bit volatile unsigned integer.
typedef volatile uint8_t	vuint8;
//! 16 bit volatile unsigned integer.
typedef volatile uint16_t	vuint16;
//! 32 bit volatile unsigned integer.
typedef volatile uint32_t	vuint32;
//! 64 bit volatile unsigned integer.
typedef volatile uint64_t	vuint64;

//! 8 bit volatile signed integer.
typedef volatile int8_t		vint8;
//! 16 bit volatile signed integer.
typedef volatile int16_t	vint16;
//! 32 bit volatile signed integer.
typedef volatile int32_t	vint32;
//! 64 bit volatile signed integer.
typedef volatile int64_t	vint64;

//! 32 bit volatile signed floating point number.
typedef volatile float32        vfloat32;
//! 64 bit volatile signed floating point number.
typedef volatile float64        vfloat64;

//! 8 bit unsigned integer.
typedef uint8_t		byte;

//! 8 bit unsigned integer.
typedef uint8_t		u8;
//! 16 bit unsigned integer.
typedef uint16_t	u16;
//! 32 bit unsigned integer.
typedef uint32_t	u32;
//! 64 bit unsigned integer.
typedef uint64_t	u64;

//! 8 bit signed integer.
typedef int8_t		s8;
//! 16 bit signed integer.
typedef int16_t		s16;
//! 32 bit signed integer.
typedef int32_t		s32;
//! 64 bit signed integer.
typedef int64_t		s64;

//! 8 bit volatile unsigned integer.
typedef volatile u8          vu8;
//! 16 bit volatile unsigned integer.
typedef volatile u16         vu16;
//! 32 bit volatile unsigned integer.
typedef volatile u32         vu32;
//! 64 bit volatile unsigned integer.
typedef volatile u64         vu64;

//! 8 bit volatile signed integer.
typedef volatile s8           vs8;
//! 16 bit volatile signed integer.
typedef volatile s16          vs16;
//! 32 bit volatile signed integer.
typedef volatile s32          vs32;
//! 64 bit volatile signed integer.
typedef volatile s64          vs64;


//All of this has been taken out of libnds header files.

#define GFX_CONTROL           (*(vu16*) 0x04000060)

#define GFX_FIFO              (*(vu32*) 0x04000400)
#define GFX_STATUS            (*(vu32*) 0x04000600)
#define GFX_COLOR             (*(vu32*) 0x04000480)

#define GFX_VERTEX10          (*(vu32*) 0x04000490)
#define GFX_VERTEX_XY         (*(vu32*) 0x04000494)
#define GFX_VERTEX_XZ         (*(vu32*) 0x04000498)
#define GFX_VERTEX_YZ         (*(vu32*) 0x0400049C)
#define GFX_VERTEX_DIFF       (*(vu32*) 0x040004A0)

#define GFX_VERTEX16          (*(vu32*) 0x0400048C)
#define GFX_TEX_COORD         (*(vu32*) 0x04000488)
#define GFX_TEX_FORMAT        (*(vu32*) 0x040004A8)
#define GFX_PAL_FORMAT        (*(vu32*) 0x040004AC)

#define GFX_CLEAR_COLOR       (*(vu32*) 0x04000350)
#define GFX_CLEAR_DEPTH       (*(vu16*) 0x04000354)

#define GFX_LIGHT_VECTOR      (*(vu32*) 0x040004C8)
#define GFX_LIGHT_COLOR       (*(vu32*) 0x040004CC)
#define GFX_NORMAL            (*(vu32*) 0x04000484)

#define GFX_DIFFUSE_AMBIENT   (*(vu32*) 0x040004C0)
#define GFX_SPECULAR_EMISSION (*(vu32*) 0x040004C4)
#define GFX_SHININESS         (*(vu32*) 0x040004D0)

#define GFX_POLY_FORMAT       (*(vu32*) 0x040004A4)
#define GFX_ALPHA_TEST        (*(vu16*) 0x04000340)

#define GFX_BEGIN			(*(vu32*) 0x04000500)
#define GFX_END				(*(vu32*) 0x04000504)
#define GFX_FLUSH			(*(vu32*) 0x04000540)
#define GFX_VIEWPORT		(*(vu32*) 0x04000580)
#define GFX_TOON_TABLE		((vu16*)  0x04000380)
#define GFX_EDGE_TABLE		((vu16*)  0x04000330)
#define GFX_FOG_COLOR		(*(vu32*) 0x04000358)
#define GFX_FOG_OFFSET		(*(vu32*) 0x0400035C)
#define GFX_FOG_TABLE		((vu8*)   0x04000360)
#define GFX_BOX_TEST		(*(vs32*)  0x040005C0)
#define GFX_POS_TEST		(*(vu32*) 0x040005C4)
#define GFX_POS_RESULT		((vs32*)   0x04000620)
#define GFX_VEC_TEST		(*(vu32*) 0x040005C8)
#define GFX_VEC_RESULT      ((vs16*)   0x04000630)

#define GFX_BUSY (GFX_STATUS & BIT(27))

#define GFX_VERTEX_RAM_USAGE	(*(u16*)  0x04000606)
#define GFX_POLYGON_RAM_USAGE	(*(u16*)  0x04000604)

#define GFX_CUTOFF_DEPTH		(*(u16*)0x04000610)

// Matrix processor control

#define MATRIX_CONTROL		(*(vu32*)0x04000440)
#define MATRIX_PUSH			(*(vu32*)0x04000444)
#define MATRIX_POP			(*(vu32*)0x04000448)
#define MATRIX_SCALE		(*(vs32*) 0x0400046C)
#define MATRIX_TRANSLATE	(*(vs32*) 0x04000470)
#define MATRIX_RESTORE		(*(vu32*)0x04000450)
#define MATRIX_STORE		(*(vu32*)0x0400044C)
#define MATRIX_IDENTITY		(*(vu32*)0x04000454)
#define MATRIX_LOAD4x4		(*(vs32*) 0x04000458)
#define MATRIX_LOAD4x3		(*(vs32*) 0x0400045C)
#define MATRIX_MULT4x4		(*(vs32*) 0x04000460)
#define MATRIX_MULT4x3		(*(vs32*) 0x04000464)
#define MATRIX_MULT3x3		(*(vs32*) 0x04000468)

//matrix operation results
#define MATRIX_READ_CLIP		((vs32*) (0x04000640))
#define MATRIX_READ_VECTOR		((vs32*) (0x04000680))
#define POINT_RESULT			((vs32*) (0x04000620))
#define VECTOR_RESULT			((vu16*)(0x04000630))

#define RGB15(r,g,b)  ((r)|((g)<<5)|((b)<<10))


//////////////////////////////////////////////////////////////////////
// Fixed point / floating point / integer conversion macros
//////////////////////////////////////////////////////////////////////

typedef uint16 fixed12d3; /*!< \brief Used for depth (glClearDepth, glCutoffDepth) */


#define int_to_12d3(n)    ((n) << 3) /*!< \brief convert int to fixed12d3 */
#define float_to_12d3(n)  ((fixed12d3)((n) * (1 << 3))) /*!< \brief convert float to fixed12d3 */
#define GL_MAX_DEPTH      0x7FFF /*!< \brief the maximum value for type fixed12d3 */

//////////////////////////////////////////////////////////////////////

#define inttof32(n)          ((n) << 12) /*!< \brief convert int to f32 */
#define f32toint(n)          ((n) >> 12) /*!< \brief convert f32 to int */
#define floattof32(n)        ((int)((n) * (1 << 12))) /*!< \brief convert float to f32 */
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12)) /*!< \brief convert f32 to float */

typedef short t16;        /*!< \brief text coordinate 12.4 fixed point */
#define f32tot16(n)          ((t16)(n >> 8)) /*!< \brief convert f32 to t16 */
#define inttot16(n)          ((n) << 4) /*!< \brief convert int to t16 */
#define t16toint(n)          ((n) >> 4) /*!< \brief convert t16 to int */
#define floattot16(n)        ((t16)((n) * (1 << 4))) /*!< \brief convert float to t16 */
#define TEXTURE_PACK(u,v)    (((u) & 0xFFFF) | ((v) << 16)) /*!< \brief Pack 2 t16 texture coordinate values into a 32bit value */

typedef short int v16;       /*!< \brief vertex 4.12 fixed format */
#define inttov16(n)          ((n) << 12) /*!< \brief convert int to v16 */
#define f32tov16(n)          (n) /*!< \brief f32 to v16 */
#define v16toint(n)          ((n) >> 12) /*!< \brief convert v16 to int */
#define floattov16(n)        ((v16)((n) * (1 << 12))) /*!< \brief convert float to v16 */
#define VERTEX_PACK(x,y)     (((x) & 0xFFFF) | ((y) << 16)) /*!< \brief Pack to v16 values into one 32bit value */

typedef short int v10;       /*!< \brief normal .10 fixed point, NOT USED FOR 10bit VERTEXES!!!*/
#define inttov10(n)          ((n) << 9) /*!< \brief convert int to v10 */
#define f32tov10(n)          ((v10)(n >> 3)) /*!< \brief convert f32 to v10 */
#define v10toint(n)          ((n) >> 9) /*!< \brief convert v10 to int */
#define floattov10(n)        ((n>.998) ? 0x1FF : ((v10)((n)*(1<<9)))) /*!< \brief convert float to v10 */
#define NORMAL_PACK(x,y,z)   (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | ((z) << 20)) /*!< \brief Pack 3 v10 normals into a 32bit value */


/*! \brief Enums selecting polygon draw mode<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A><BR>
related functions: glBegin() */
typedef enum {
	GL_TRIANGLES      = 0, /*!< draw triangles with each 3 vertices defining a triangle */
	GL_QUADS          = 1, /*!< draw quads with each 4 vertices defining a quad */
	GL_TRIANGLE_STRIP = 2, /*!< draw triangles with the first triangle defined by 3 vertices, then each additional triangle being defined by one additional vertex */
	GL_QUAD_STRIP     = 3, /*!< draw quads with the first quad being defined by 4 vertices, then each additional triangle being defined by 2 vertices. */
	GL_TRIANGLE       = 0, /*!< same as GL_TRIANGLES, old non-OpenGL version */
	GL_QUAD           = 1  /*!< same as GL_QUADS, old non-OpenGL version */
} GL_GLBEGIN_ENUM;


//---------------------------------------------------------------------------------
//Fifo commands
//---------------------------------------------------------------------------------

#define FIFO_COMMAND_PACK(c1,c2,c3,c4) (((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1)) /*!< \brief packs four packed commands into a 32bit command for sending to the GFX FIFO */

#define REG2ID(r)				(u8)( ( ((u32)(&(r)))-0x04000400 ) >> 2 ) /*!< \brief converts a GFX command for use in a packed command list */

#define FIFO_NOP				REG2ID(GFX_FIFO) /*!< \brief packed command for nothing, just here to pad your command lists */
#define FIFO_STATUS				REG2ID(GFX_STATUS) /*!< \brief packed command for geometry engine status register<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dstatus">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dstatus</A> */
#define FIFO_COLOR				REG2ID(GFX_COLOR) /*!< \brief packed command for vertex color directly<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes</A> */

#define FIFO_VERTEX16			REG2ID(GFX_VERTEX16) /*!< \brief packed command for a vertex with 3 16bit paramaters (and 16bits of padding)<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A> */
#define FIFO_VERTEX10			REG2ID(GFX_VERTEX10) /*!< \brief packed command for a vertex with 3 10bit paramaters (and 2bits of padding)<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A> */
#define FIFO_VERTEX_XY			REG2ID(GFX_VERTEX_XY) /*!< \brief packed command for a vertex with 2 16bit paramaters (reusing current last-set vertex z value)<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A> */
#define FIFO_VERTEX_XZ			REG2ID(GFX_VERTEX_XZ) /*!< \brief packed command for a vertex with 2 16bit paramaters (reusing current last-set vertex y value)<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A> */
#define FIFO_VERTEX_YZ			REG2ID(GFX_VERTEX_YZ) /*!< \brief packed command for a vertex with 2 16bit paramaters (reusing current last-set vertex x value)<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A> */
#define FIFO_TEX_COORD			REG2ID(GFX_TEX_COORD) /*!< \brief packed command for a texture coordinate<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtexturecoordinates">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtexturecoordinates</A> */
#define FIFO_TEX_FORMAT			REG2ID(GFX_TEX_FORMAT) /*!< \brief packed command for texture format<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureformats">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureformats</A> */
#define FIFO_PAL_FORMAT			REG2ID(GFX_PAL_FORMAT) /*!< \brief packed command for texture palette attributes<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes</A> */

#define FIFO_CLEAR_COLOR		REG2ID(GFX_CLEAR_COLOR) /*!< \brief packed command for clear color of the rear plane<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3drearplane">GBATEK http://nocash.emubase.de/gbatek.htm#ds3drearplane</A> */
#define FIFO_CLEAR_DEPTH		REG2ID(GFX_CLEAR_DEPTH) /*!< \brief sets depth of the rear plane<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3drearplane">GBATEK http://nocash.emubase.de/gbatek.htm#ds3drearplane</A> */

#define FIFO_LIGHT_VECTOR		REG2ID(GFX_LIGHT_VECTOR) /*!< \brief packed command for direction of a light source<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_LIGHT_COLOR		REG2ID(GFX_LIGHT_COLOR) /*!< \brief packed command for color for a light<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_NORMAL				REG2ID(GFX_NORMAL) /*!< \brief packed command for normal for following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */

#define FIFO_DIFFUSE_AMBIENT	REG2ID(GFX_DIFFUSE_AMBIENT) /*!< \brief packed command for setting diffuse and ambient material properties for the following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_SPECULAR_EMISSION	REG2ID(GFX_SPECULAR_EMISSION) /*!< \brief packed command for setting specular and emmissive material properties for the following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_SHININESS			REG2ID(GFX_SHININESS) /*!< \brief packed command for setting the shininess table to be used for the following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */

#define FIFO_POLY_FORMAT		REG2ID(GFX_POLY_FORMAT) /*!< \brief packed command for setting polygon attributes<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes</A> */

#define FIFO_BEGIN				REG2ID(GFX_BEGIN) /*!< \brief packed command that starts a polygon vertex list<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A>*/
#define FIFO_END				REG2ID(GFX_END) /*!< \brief packed command that has no discernable effect, it's probably best to never use it since it bloats the size of the list.<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A>*/
#define FIFO_FLUSH				REG2ID(GFX_FLUSH) /*!< \brief packed command that has the same effect as swiWaitForVBlank()<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A> */
#define FIFO_VIEWPORT			REG2ID(GFX_VIEWPORT) /*!< \brief packed command for setting viewport<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A> */

#endif