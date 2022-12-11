///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for optimized trees.
 *	\file		OPC_OptimizedTree.cpp
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A standard AABB tree.
 *
 *	\class		AABBCollisionTree
 *	\author		Pierre Terdiman
 *	\version	1.2
 *	\date		March, 20, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A no-leaf AABB tree.
 *
 *	\class		AABBNoLeafTree
 *	\author		Pierre Terdiman
 *	\version	1.2
 *	\date		March, 20, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A quantized AABB tree.
 *
 *	\class		AABBQuantizedTree
 *	\author		Pierre Terdiman
 *	\version	1.2
 *	\date		March, 20, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A quantized no-leaf AABB tree.
 *
 *	\class		AABBQuantizedNoLeafTree
 *	\author		Pierre Terdiman
 *	\version	1.2
 *	\date		March, 20, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
//#include "stdafx.h"
#pragma hdrstop

#include "Opcode.h"

using namespace Opcode;

#define CHECKALLOC(x)		if(!x) return false;

//! Compilation flag:
//! - true to fix quantized boxes (i.e. make sure they enclose the original ones)
//! - false to see the effects of quantization errors (faster, but wrong results in some cases)
static bool gFixQuantized = true;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Builds a "no-leaf" tree from a standard one. This is a tree whose leaf nodes have been removed.
 *
 *	Layout for no-leaf trees:
 *
 *	Node:
 *			- box
 *			- P pointer => a node (LSB=0) or a primitive (LSB=1)
 *			- N pointer => a node (LSB=0) or a primitive (LSB=1)
 *
 *	\relates	AABBNoLeafNode
 *	\fn			_BuildNoLeafTree(AABBNoLeafNode* linear, const unsigned int boxid, unsigned int& curid, const AABBTreeNode* curnode)
 *	\param		linear		[in] base address of destination nodes
 *	\param		boxid		[in] index of destination node
 *	\param		curid		[in] current running index
 *	\param		curnode		[in] current node from input tree
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void _BuildNoLeafTree(AABBNoLeafNode* linear, const unsigned int boxid, unsigned int& curid, const AABBTreeNode* curnode)
{
	const AABBTreeNode* P = curnode->GetPos();
	const AABBTreeNode* N = curnode->GetNeg();
	// Leaf nodes here?!
	ASSERT(P);
	ASSERT(N);
	// Internal node => keep the box
	curnode->GetAABB()->GetCenter(linear[boxid].mAABB.mCenter);
	curnode->GetAABB()->GetExtents(linear[boxid].mAABB.mExtents);

	if(P->IsLeaf())
	{
		// The input tree must be complete => i.e. one primitive/leaf
		ASSERT(P->GetNbPrimitives()==1);
		// Get the primitive index from the input tree
		unsigned int PrimitiveIndex = P->GetPrimitives()[0];
		// Setup prev box data as the primitive index, marked as leaf
		linear[boxid].mData = (PrimitiveIndex<<1)|1;
	}
	else
	{
		// Get a _new_ id for positive child
		unsigned int PosID = curid++;
		// Setup box data
		linear[boxid].mData = (uintptr_t)&linear[PosID];
		// Make sure it's not marked as leaf
		ASSERT(!(linear[boxid].mData&1));
		// Recurse
		_BuildNoLeafTree(linear, PosID, curid, P);
	}

	if(N->IsLeaf())
	{
		// The input tree must be complete => i.e. one primitive/leaf
		ASSERT(N->GetNbPrimitives()==1);
		// Get the primitive index from the input tree
		unsigned int PrimitiveIndex = N->GetPrimitives()[0];
		// Setup prev box data as the primitive index, marked as leaf
		linear[boxid].mData2 = (PrimitiveIndex<<1)|1;
	}
	else
	{
		// Get a _new_ id for positive child
		unsigned int NegID = curid++;
		// Setup box data
		linear[boxid].mData2 = (uintptr_t)&linear[NegID];
		// Make sure it's not marked as leaf
		ASSERT(!(linear[boxid].mData2&1));
		// Recurse
		_BuildNoLeafTree(linear, NegID, curid, N);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABBNoLeafTree::AABBNoLeafTree() : mNodes(nullptr)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABBNoLeafTree::~AABBNoLeafTree()
{
	xr_free(mNodes);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Builds the collision tree from a generic AABB tree.
 *	\param		tree			[in] generic AABB tree
 *	\return		true if success
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABBNoLeafTree::Build(AABBTree* tree)
{
	// Checkings
	if(!tree)	return false;
	// Check the input tree is complete
	unsigned int NbTriangles	= tree->GetNbPrimitives();
	unsigned int NbNodes		= tree->GetNbNodes();
	if(NbNodes!=NbTriangles*2-1)	return false;

	// Get nodes
	mNbNodes	= NbTriangles-1;
	mNodes		= xr_alloc<AABBNoLeafNode>(mNbNodes);
	CHECKALLOC	(mNodes);
	ZeroMemory	(mNodes,mNbNodes*sizeof(AABBNoLeafNode));

	// Build the tree
	unsigned int CurID = 1;
	_BuildNoLeafTree(mNodes, 0, CurID, tree);
	ASSERT(CurID==mNbNodes);

#ifdef __ICECORE_H__
	Log("Original tree: %d nodes, depth %d\n", NbNodes, tree->ComputeDepth());
	Log("AABB quantized tree: %d nodes, %d bytes - Alignment: %d\n", mNbNodes, GetUsedBytes(), Alignment(unsigned int(mNodes)));
#endif

	return true;
}

// Quantization notes:
// - We could use the highest bits of mData to store some more quantized bits. Dequantization code
//   would be slightly more complex, but number of overlap tests would be reduced (and anyhow those
//   bits are currently wasted). Of course it's not possible if we move to 16 bits mData.
// - Something like "16 bits floats" could be tested, to bypass the int-to-float conversion.
// - A dedicated BV-BV test could be used, dequantizing while testing for overlap. (i.e. it's some
//   lazy-dequantization which may save some work in case of early exits). At the very least some
//   muls could be saved by precomputing several more matrices. But maybe not worth the pain.
// - Do we need to dequantize anyway? Not doing the extents-related muls only implies the box has
//   been scaled, for example.
// - The deeper we move into the hierarchy, the smaller the extents should be. May not need a fixed
//   number of quantization bits. Even better, could probably be best delta-encoded.

// Find max values (could use the first node only with min/max boxes)
#define FIND_MAX_VALUES																			\
	/* Get max values */																		\
	Point CMax(flt_min, flt_min, flt_min);												\
	Point EMax(flt_min, flt_min, flt_min);												\
	for(unsigned int i=0;i<mNbNodes;i++)																\
	{																							\
		if(_abs(Nodes[i].mAABB.mCenter.x)>CMax.x)	CMax.x = _abs(Nodes[i].mAABB.mCenter.x);	\
		if(_abs(Nodes[i].mAABB.mCenter.y)>CMax.y)	CMax.y = _abs(Nodes[i].mAABB.mCenter.y);	\
		if(_abs(Nodes[i].mAABB.mCenter.z)>CMax.z)	CMax.z = _abs(Nodes[i].mAABB.mCenter.z);	\
		if(_abs(Nodes[i].mAABB.mExtents.x)>EMax.x)	EMax.x = _abs(Nodes[i].mAABB.mExtents.x);	\
		if(_abs(Nodes[i].mAABB.mExtents.y)>EMax.y)	EMax.y = _abs(Nodes[i].mAABB.mExtents.y);	\
		if(_abs(Nodes[i].mAABB.mExtents.z)>EMax.z)	EMax.z = _abs(Nodes[i].mAABB.mExtents.z);	\
	}

#define INIT_QUANTIZATION							\
	unsigned int nbc=15;	/* Keep one bit for sign */		\
	unsigned int nbe=15;	/* Keep one bit for fix */		\
	if(!gFixQuantized) nbe++;						\
													\
	/* Compute quantization coeffs */				\
	Point CQuantCoeff, EQuantCoeff;					\
	CQuantCoeff.x = float((1<<nbc)-1)/CMax.x;		\
	CQuantCoeff.y = float((1<<nbc)-1)/CMax.y;		\
	CQuantCoeff.z = float((1<<nbc)-1)/CMax.z;		\
	EQuantCoeff.x = float((1<<nbe)-1)/EMax.x;		\
	EQuantCoeff.y = float((1<<nbe)-1)/EMax.y;		\
	EQuantCoeff.z = float((1<<nbe)-1)/EMax.z;		\
	/* Compute and save dequantization coeffs */	\
	mCenterCoeff.x = 1.0f / CQuantCoeff.x;			\
	mCenterCoeff.y = 1.0f / CQuantCoeff.y;			\
	mCenterCoeff.z = 1.0f / CQuantCoeff.z;			\
	mExtentsCoeff.x = 1.0f / EQuantCoeff.x;			\
	mExtentsCoeff.y = 1.0f / EQuantCoeff.y;			\
	mExtentsCoeff.z = 1.0f / EQuantCoeff.z;

#define PERFORM_QUANTIZATION														\
	/* Quantize */																	\
	((float*)mNodes[i].mAABB.mCenter)[0] = signed short(Nodes[i].mAABB.mCenter.x * CQuantCoeff.x);	\
	((float*)mNodes[i].mAABB.mCenter)[1] = signed short(Nodes[i].mAABB.mCenter.y * CQuantCoeff.y);	\
	((float*)mNodes[i].mAABB.mCenter)[2] = signed short(Nodes[i].mAABB.mCenter.z * CQuantCoeff.z);	\
	((float*)mNodes[i].mAABB.mExtents)[0] = unsigned short(Nodes[i].mAABB.mExtents.x * EQuantCoeff.x);	\
	((float*)mNodes[i].mAABB.mExtents)[1] = unsigned short(Nodes[i].mAABB.mExtents.y * EQuantCoeff.y);	\
	((float*)mNodes[i].mAABB.mExtents)[2] = unsigned short(Nodes[i].mAABB.mExtents.z * EQuantCoeff.z);	\
	/* Fix quantized boxes */														\
	if(gFixQuantized)																\
	{																				\
		/* Make sure the quantized box is still valid */							\
		Point Max = Nodes[i].mAABB.mCenter + Nodes[i].mAABB.mExtents;				\
		Point Min = Nodes[i].mAABB.mCenter - Nodes[i].mAABB.mExtents;				\
		/* For each axis */															\
		for(unsigned int j=0;j<3;j++)														\
		{	/* Dequantize the box center */											\
			float qc = float(((float*)mNodes[i].mAABB.mCenter)[j]) * ((float*)mCenterCoeff)[j];			\
			bool FixMe=true;														\
			do																		\
			{	/* Dequantize the box extent */										\
				float qe = float(((float*)mNodes[i].mAABB.mExtents)[j]) * ((float*)mExtentsCoeff)[j];	\
				/* Compare real & dequantized values */								\
				if(qc+qe<((float*)Max)[j] || qc-qe>((float*)Min)[j])	((float*)mNodes[i].mAABB.mExtents)[j]++;	\
				else								FixMe=false;					\
				/* Prevent wrapping */												\
				if(!((float*)mNodes[i].mAABB.mExtents)[j])									\
				{																	\
					((float*)mNodes[i].mAABB.mExtents)[j]=0xffff;								\
					FixMe=false;													\
				}																	\
			}while(FixMe);															\
		}																			\
	}

#define REMAP_DATA(member)											\
	/* Fix data */													\
	Data = Nodes[i].member;											\
	if(!(Data&1))													\
	{																\
		/* Compute box number */									\
		size_t Nb = (Data - uintptr_t(Nodes))/Nodes[i].GetNodeSize();	\
		Data = uintptr_t(&mNodes[Nb]);									\
	}																\
	/* ...remapped */												\
	mNodes[i].member = Data;

