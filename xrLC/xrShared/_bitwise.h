#ifndef _BITWISE_
#define _BITWISE_
#pragma once

// float values defines
#define fdSGN	0x080000000		// mask for sign bit
#define fdMABS  0x07FFFFFFF		// mask for absolute value (~sgn)
#define fdMANT  0x0007FFFFF		// mask for mantissa
#define fdEXPO	0x07F800000		// mask for exponent
#define fdONE	0x03F800000     // 1.0f
#define fdHALF	0x03F000000     // 0.5f
#define fdTWO	0x040000000     // 2.0
#define fdOOB	0x000000000     // "out of bounds" value
#define fdNAN	0x07fffffff     // "Not a number" value
#define fdMAX	0x07F7FFFFF     // FLT_MAX
#define fdRLE10	0x03ede5bdb     // 1/ln10

// integer math on floats
#ifdef	_M_AMD64
	IC bool negative(const float f)		{ return f<0;	}
	IC bool positive(const float f)		{ return f>=0;	}
	IC void set_negative(float &f)		{ f = -fabsf(f); }
	IC void set_positive(float &f)		{ f = fabsf(f);	}
#else
	IC BOOL negative(const float &f)	{ return (*((unsigned*)(&f))&fdSGN);	}
	IC BOOL positive(const float &f)	{ return (*((unsigned*)(&f))&fdSGN)==0;	}
	IC void set_negative(float &f)		{ (*(unsigned*)(&f)) |= fdSGN;			}
	IC void set_positive(float &f)		{ (*(unsigned*)(&f)) &= ~fdSGN;			}
#endif

/*
 * Here are a few nice tricks for 2's complement based machines
 * that I discovered a few months ago.
 */
IC	int		btwLowestBitMask(int v)		{	return (v & -v);	}
IC	u32		btwLowestBitMask(u32 x)		{   return x & ~(x-1);	}

/* Ok, so now we are cooking on gass. Here we use this function for some */
/* rather useful utility functions */
IC	bool	btwIsPow2(int v)			{ return (btwLowestBitMask(v) == v); }
IC	bool	btwIsPow2(u32 v)			{ return (btwLowestBitMask(v) == v); }

IC	int		btwPow2_Ceil(int v)
{
	int i = btwLowestBitMask(v);
	while(i < v) i <<= 1;
	return i;
}
IC	u32		btwPow2_Ceil(u32 v)
{
	u32 i = btwLowestBitMask(v);
	while(i < v) i <<= 1;
	return i;
}

ICF int iFloor (float x)
{
    int a			= *(const int*)(&x);
    int exponent	= (127 + 31) - ((a >> 23) & 0xFF);
    int r			= (((u32)(a) << 8) | (1U << 31)) >> exponent;
    exponent		+= 31-127;
    {
        int imask	=	(!(((( (1<<(exponent)))-1)>>8)&a));
        exponent	-=	(31-127)+32;
        exponent	>>=	31;
        a			>>=	31;
        r			-=	(imask&a);
        r			&=	exponent;
        r			^=	a;
    }
    return r;
}

/* intCeil() is a non-interesting variant, since effectively
   ceil(x) == -floor(-x)
*/
ICF int iCeil (float x)
{
    int a			= (*(const int*)(&x));
    int exponent	= (127 + 31) - ((a >> 23) & 0xFF);
    int r			= (((u32)(a) << 8) | (1U << 31)) >> exponent;
    exponent		+= 31-127;
    {
        int imask	=	(!(((( (1<<(exponent)))-1)>>8)&a));
        exponent	-=	(31-127)+32;
        exponent	>>=	31;
        a			=	~((a-1)>>31);		/* change sign */
        r			-=	(imask&a);
        r			&=	exponent;
        r			^=	a;
        r			=	-r;                 /* change sign */
    }
    return r;								/* r = (int)(ceil(f)) */
}

// Validity checks
IC bool fis_gremlin		( const float &f ) 
{
	u8		value = u8(((*(int*)&f & 0x7f800000)>>23)-0x20);
    return	value > 0xc0;
}
IC bool fis_denormal	( const float &f ) 
{
  return !(*(int*)&f & 0x7f800000);
}

// Only for [0..1] (positive) range 
IC float apx_asin	(const float x)
{
	const float c1 = 0.892399f;
	const float c3 = 1.693204f;
	const float c5 =-3.853735f;
	const float c7 = 2.838933f;
	
	const float x2 = x * x;
	const float d = x * (c1 + x2 * (c3 + x2 * (c5 + x2 * c7)));
	
	return d;
}
// Only for [0..1] (positive) range 
IC float apx_acos	(const float x)
{
	return PI_DIV_2 - apx_asin(x);
}

#endif