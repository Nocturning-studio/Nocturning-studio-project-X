///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a handy triangle class.
 *	\file		IceTriangle.h
 *	\author		Pierre Terdiman
 *	\date		January, 17, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICETRIANGLE_H__
#define __ICETRIANGLE_H__

	// An indexed triangle class.
	class OPCODE_API IndexedTriangle
	{
		public:
		//! Constructor
		inline					IndexedTriangle()									{}
		//! Constructor
		inline					IndexedTriangle(unsigned int r0, unsigned int r1, unsigned int r2)	{ mVRef[0]=r0; mVRef[1]=r1; mVRef[2]=r2; }
		//! Copy constructor
		inline					IndexedTriangle(const IndexedTriangle& triangle)
								{
									mVRef[0] = triangle.mVRef[0];
									mVRef[1] = triangle.mVRef[1];
									mVRef[2] = triangle.mVRef[2];
								}
		//! Destructor
		inline					~IndexedTriangle()									{}
		//! Vertex-references
				unsigned int			mVRef[3];

		// Methods
				bool			IsDegenerate()	const
				{
					if (mVRef[0] == mVRef[1])	return true;
					if (mVRef[1] == mVRef[2])	return true;
					if (mVRef[2] == mVRef[0])	return true;
					return false;
				}
	};

#endif // __ICETRIANGLE_H__
