///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for 3D vectors.
 *	\file		IcePoint.cpp
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	3D point.
 *
 *	The name is "Point" instead of "Vector" since a vector3 is N-dimensional, whereas a point is an implicit "vector3 of dimension 3".
 *	So the choice was between "Point" and "Vector3", the first one looked better (IMHO).
 *
 *	Some people, then, use a typedef to handle both points & vectors using the same class: typedef Point Vector3;
 *	This is bad since it opens the door to a lot of confusion while reading the code. I know it may sounds weird but check this out:
 *
 *	\code
 *		Point P0,P1 = some 3D points;
 *		Point Delta = P1 - P0;
 *	\endcode
 *
 *	This compiles fine, although you should have written:
 *
 *	\code
 *		Point P0,P1 = some 3D points;
 *		Vector3 Delta = P1 - P0;
 *	\endcode
 *
 *	Subtle things like this are not caught at compile-time, and when you find one in the code, you never know whether it's a mistake
 *	from the author or something you don't get.
 *
 *	One way to handle it at compile-time would be to use different classes for Point & Vector3, only overloading operator "-" for vectors.
 *	But then, you get a lot of redundant code in thoses classes, and basically it's really a lot of useless work.
 *
 *	Another way would be to use homogeneous points: w=1 for points, w=0 for vectors. That's why the HPoint class exists. Now, to store
 *	your model's vertices and in most cases, you really want to use Points to save ram.
 *
 *	\class		Point
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
//#include "stdafx.h"
#pragma hdrstop

#include "Opcode.h"

using namespace IceMaths;

// Cast operator
// WARNING: not inlined
//Point::operator HPoint() const	{ return HPoint(x, y, z, 0.0f); }

Point& Point::Refract(const Point& eye, const Point& n, float refractindex, Point& refracted)
{
	//	Point EyePt = eye position
	//	Point p = current vertex
	//	Point n = vertex normal
	//	Point rv = refracted vector3
	//	Eye vector3 - doesn't need to be normalized
	Point Env;
	Env.x = eye.x - x;
	Env.y = eye.y - y;
	Env.z = eye.z - z;

	float NDotE = n|Env;
	float NDotN = n|n;
	NDotE /= refractindex;

	// Refracted vector3
	refracted = n*NDotE - Env*NDotN;

	return *this;
}



