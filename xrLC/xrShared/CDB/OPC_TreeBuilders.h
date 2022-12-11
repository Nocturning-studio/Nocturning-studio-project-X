///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for tree builders.
 *	\file		OPC_TreeBuilders.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPC_TREEBUILDERS_H__
#define __OPC_TREEBUILDERS_H__

#include "opc_aabbtree.h"
#include "..\xrPool.h"

	//! Tree splitting rules
	enum SplittingRules			{
		// Tree
		SPLIT_COMPLETE			= (1<<0),		//!< Build a complete tree (2*N-1 nodes)
		// Primitive split
		SPLIT_LARGESTAXIS		= (1<<1),		//!< Split along the largest axis
		SPLIT_SPLATTERPOINTS	= (1<<2),		//!< Splatter primitive centers (QuickCD-style)
		SPLIT_BESTAXIS			= (1<<3),		//!< Try largest axis, then second, then last
		SPLIT_BALANCED			= (1<<4),		//!< Try to keep a well-balanced tree
		SPLIT_FIFTY				= (1<<5),		//!< Arbitrary 50-50 split
		// Node split
		SPLIT_GEOMCENTER		= (1<<6),		//!< Split at geometric center (else split in the middle)
		//
		SPLIT_FORCE_DWORD		= 0x7fffffff
	};



	class OPCODE_API AABBTreeBuilder
	{
		public:
		//! Constructor
												AABBTreeBuilder() :
													mLimit(0),
													mRules(SPLIT_FORCE_DWORD),
													mNbPrimitives(0),
													mCount(0),
													mNbInvalidSplits(0)		{}
		//! Destructor
		virtual									~AABBTreeBuilder()			{}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the AABB of a set of primitives.
		 *	\param		primitives		[in] list of indices of primitives
		 *	\param		nb_prims		[in] number of indices
		 *	\param		global_box		[out] global AABB enclosing the set of input primitives
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual						bool		ComputeGlobalBox(const unsigned int* primitives, unsigned int nb_prims, AABB& global_box)	const	= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the splitting value along a given axis for a given primitive.
		 *	\param		index			[in] index of the primitive to split
		 *	\param		axis			[in] axis index (0,1,2)
		 *	\return		splitting value
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual						float		GetSplittingValue(unsigned int index, unsigned int axis)	const	= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes the splitting value along a given axis for a given node.
		 *	\param		primitives		[in] list of indices of primitives
		 *	\param		nb_prims		[in] number of indices
		 *	\param		global_box		[in] global AABB enclosing the set of input primitives
		 *	\param		axis			[in] axis index (0,1,2)
		 *	\return		splitting value
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual						float		GetSplittingValueEx(const unsigned int* primitives, unsigned int nb_prims, const AABB& global_box, unsigned int axis)	const
									{
										// Default split value = middle of the axis (using only the box)
										return global_box.GetCenter(axis);
									}

									unsigned int		mLimit;				//!< Limit number of primitives / node
									unsigned int		mRules;				//!< Building/Splitting rules (a combination of flags)
									unsigned int		mNbPrimitives;		//!< Total number of primitives.
		// Stats
		inline						void		SetCount(unsigned int nb)				{ mCount=nb;				}
		inline						void		IncreaseCount(unsigned int nb)		{ mCount+=nb;				}
		inline						unsigned int		GetCount()				const	{ return mCount;			}
		inline						void		SetNbInvalidSplits(unsigned int nb)	{ mNbInvalidSplits=nb;		}
		inline						void		IncreaseNbInvalidSplits()		{ mNbInvalidSplits++;		}
		inline						unsigned int		GetNbInvalidSplits()	const	{ return mNbInvalidSplits;	}

		private:					
									unsigned int		mCount;				//!< Stats: number of nodes created
									unsigned int		mNbInvalidSplits;	//!< Stats: number of invalid splits
		public:
				poolSS<AABBTreeNode,16*1024>	mPOOL		;
		inline					AABBTreeNode*	node_alloc	()					{return mPOOL.create();		}
		inline						void		node_destroy(AABBTreeNode* &n)	{return mPOOL.destroy(n);	}
	};


#pragma warning(disable:4512)
	class OPCODE_API AABBTreeOfTrianglesBuilder : public AABBTreeBuilder
	{
		public:
		//! Constructor
												AABBTreeOfTrianglesBuilder() : mTriList(nullptr), mVerts(nullptr), mNbTriangles(0)	{}
		//! Destructor
		virtual									~AABBTreeOfTrianglesBuilder()													{}

		virtual	bool		ComputeGlobalBox(const unsigned int* primitives, unsigned int nb_prims, AABB& global_box)	const;
		virtual	float		GetSplittingValue(unsigned int index, unsigned int axis)	const;
		virtual	float		GetSplittingValue(const unsigned int* primitives, unsigned int nb_prims, const AABB& global_box, unsigned int axis)	const;

		const				IndexedTriangle*	mTriList;			//!< Shortcut to an app-controlled list of triangles.
		const						Point*		mVerts;				//!< Shortcut to an app-controlled list of vertices.
		const						unsigned int		mNbTriangles;		//!< Total number of triangles.
	};
#pragma warning(default:4512)

#endif // __OPC_TREEBUILDERS_H__
