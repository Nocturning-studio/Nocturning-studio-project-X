///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains AABB-related code. (axis-aligned bounding box)
 *	\file		IceAABB.h
 *	\author		Pierre Terdiman
 *	\date		January, 13, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEAABB_H__
#define __ICEAABB_H__

//! Declarations of type-independent methods (implemented in the .cpp)
#define AABB_COMMON_METHODS																							\
																					\
	float	CalculateBoxArea(const icePoint& eye, const Matrix4x4& mat, float width, float height, int& num)	const;	\
	bool	IsInside(const AABB& box)																		const;

	enum AABBType
	{
		AABB_RENDER			= 0,		//!< AABB used for rendering. Not visible == not rendered.
		AABB_UPDATE			= 1,		//!< AABB used for dynamic updates. Not visible == not updated.
		AABB_FORCE_DWORD	= 0x7fffffff,
	};


	class OPCODE_API AABB
	{
		public:
		//! Constructor
		inline						AABB()	{}
		//! Destructor
		inline						~AABB()	{}

		//! Type-independent methods
									AABB_COMMON_METHODS;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Setups an AABB from min & max vectors.
		 *	\param		min			[in] the min point
		 *	\param		max			[in] the max point
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						void		SetMinMax(const Point& min, const Point& max)		{ mMin = min;		mMax = max;									}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Setups an AABB from center & extents vectors.
		 *	\param		c			[in] the center point
		 *	\param		e			[in] the extents vector3
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						void		SetCenterExtents(const Point& c, const Point& e)	{ mMin = c - e;		mMax = c + e;								}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Setups an empty AABB.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						void		SetEmpty()											{ Point p(flt_min, flt_min, flt_min);	mMin = -p; mMax = p;}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Setups a point AABB.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						void		SetPoint(const Point& pt)							{ mMin = mMax = pt;												}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Gets the size of the AABB. The size is defined as the longest extent.
		 *	\return		the size of the AABB
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						float		GetSize()								const		{ Point e; GetExtents(e);	return e.Max();	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Extends the AABB.
		 *	\param		p	[in] the next point
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						void		Extend(const Point& p)
						{
							if(p.x > mMax.x)	mMax.x = p.x;
							if(p.x < mMin.x)	mMin.x = p.x;

							if(p.y > mMax.y)	mMax.y = p.y;
							if(p.y < mMin.y)	mMin.y = p.y;

							if(p.z > mMax.z)	mMax.z = p.z;
							if(p.z < mMin.z)	mMin.z = p.z;
						}

		// Data access

		//! Get min point of the box
		inline			void		GetMin(Point& min)						const		{ min = mMin;								}
		//! Get max point of the box
		inline			void		GetMax(Point& max)						const		{ max = mMax;								}

		//! Get component of the box's min point along a given axis
		inline			float		GetMin(unsigned int axis)						const		{ return ((const float*)mMin)[axis];						}
		//! Get component of the box's max point along a given axis
		inline			float		GetMax(unsigned int axis)						const		{ return ((const float*)mMax)[axis];						}

		//! Get box center
		inline			void		GetCenter(Point& center)				const		{ center = (mMax + mMin)*0.5f;				}
		//! Get box extents
		inline			void		GetExtents(Point& extents)				const		{ extents = (mMax - mMin)*0.5f;				}

		//! Get component of the box's center along a given axis
		inline			float		GetCenter(unsigned int axis)					const		{ return (((const float*)mMax)[axis] + ((const float*)mMin)[axis])*0.5f;	}
		//! Get component of the box's extents along a given axis
		inline			float		GetExtents(unsigned int axis)					const		{ return (((const float*)mMax)[axis] - ((const float*)mMin)[axis])*0.5f;	}

		//! Get box diagonal
		inline			void		GetDiagonal(Point& diagonal)			const		{ diagonal = mMax - mMin;					}
		inline			float		GetWidth()								const		{ return mMax.x - mMin.x;					}
		inline			float		GetHeight()								const		{ return mMax.y - mMin.y;					}
		inline			float		GetDepth()								const		{ return mMax.z - mMin.z;					}

		//! Volume
		inline			float		GetVolume()								const		{ return GetWidth() * GetHeight() * GetDepth();		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the intersection between two AABBs.
		 *	\param		a		[in] the other AABB
		 *	\return		true on intersection
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline			bool		Intersect(const AABB& a)				const
						{
							if(mMax.x < a.mMin.x
							|| a.mMax.x < mMin.x
							|| mMax.y < a.mMin.y
							|| a.mMax.y < mMin.y
							|| mMax.z < a.mMin.z
							|| a.mMax.z < mMin.z)	return false;

							return true;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the 1D-intersection between two AABBs, on a given axis.
		 *	\param		a		[in] the other AABB
		 *	\param		axis	[in] the axis (0, 1, 2)
		 *	\return		true on intersection
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline			bool		Intersect(const AABB& a, unsigned int axis)	const
						{
							if(((const float*)mMax)[axis] < ((const float*)a.mMin)[axis] || ((const float*)a.mMax)[axis] < ((const float*)mMin)[axis])	return false;
							return true;
						}

		

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Checks the AABB is valid.
		 *	\return		true if the box is valid
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline			bool		IsValid()	const
						{
							// Consistency condition for (Min, Max) boxes: min < max
							if(mMin.x > mMax.x)	return false;
							if(mMin.y > mMax.y)	return false;
							if(mMin.z > mMax.z)	return false;
							return true;
						}

		//! Operator for AABB *= float. Scales the extents, keeps same center.
		inline			AABB&		operator*=(float s)
						{
							Point Center;	GetCenter(Center);
							Point Extents;	GetExtents(Extents);
							SetCenterExtents(Center, Extents * s);
							return *this;
						}

		//! Operator for AABB /= float. Scales the extents, keeps same center.
		inline			AABB&		operator/=(float s)
						{
							Point Center;	GetCenter(Center);
							Point Extents;	GetExtents(Extents);
							SetCenterExtents(Center, Extents / s);
							return *this;
						}

		//! Operator for AABB += Point. Translates the box.
		inline			AABB&		operator+=(const Point& trans)
						{
							mMin+=trans;
							mMax+=trans;
							return *this;
						}

		private:
						Point		mMin;			//!< Min point
						Point		mMax;			//!< Max point
	};

	inline void ComputeMinMax(const Point& p, Point& min, Point& max)
	{
		if(p.x > max.x)	max.x = p.x;
		if(p.x < min.x)	min.x = p.x;

		if(p.y > max.y)	max.y = p.y;
		if(p.y < min.y)	min.y = p.y;

		if(p.z > max.z)	max.z = p.z;
		if(p.z < min.z)	min.z = p.z;
	}

	inline void ComputeAABB(AABB& aabb, const Point* list, unsigned int nbpts)
	{
		if(list)
		{
			Point Maxi(flt_min, flt_min, flt_min);
			Point Mini(flt_max, flt_max, flt_max);
			while(nbpts--)
			{
				ComputeMinMax(*list++, Mini, Maxi);
			}
			aabb.SetMinMax(Mini, Maxi);
		}
	}

#endif	// __ICEAABB_H__
